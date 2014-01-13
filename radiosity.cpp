#include "radiosity.h"

// TODO : levels for debug output
patch * skyPatch = new patch(new point(0.0, 1.0, 0.0), new Vector(0.0, -1.0,  0.0), new Vector(0.0, 0.0, 1.0), 1.0, 1.0, 1.0);
patch * backPatch = new patch(new point(0.0, 0.0, -1.0), new Vector(0.0, 0.0,  1.0), new Vector(0.0, 1.0, 0.0), 0.8, 0.8, 0.8);
patch * leftPatch = new patch(new point(-1.0, 0.0, 0.0), new Vector(1.0, 0.0,  0.0), new Vector(0.0, 1.0, 0.0), 1.0, 0.3, 0.3);
patch * rightPatch = new patch(new point(1.0, 0.0, 0.0), new Vector(-1.0, 0.0,  0.0), new Vector(0.0, 1.0, 0.0), 0.3, 1.0, 0.3);
patch * bottomPatch = new patch(new point(0.0, -1.0, 0.0), new Vector(0.0, 1.0,  0.0), new Vector(0.0, 0.0, -1.0), 0.8, 0.8, 0.8);


void radiosity()
{
	light * totalLight = new light(0.0, 0.0, 0.0);
		for(int i = 0; i < ITERATIONS; i++)
		{
			for(int p = 0; p < NUMBER_OF_PATCHES; p++)
			{
				cout << "\r" << "Calculating incident lighting\t" << ((float)p / (float)NUMBER_OF_PATCHES)  * 100 << "%"<< flush;
				renderPatch(&patches[p]);	
			}
			cout << endl;
			for(int p = 0; p < NUMBER_OF_PATCHES; p++)
			{
				cout << "\r" << "Calculating excident lighting\t" << ((float)p / (float)NUMBER_OF_PATCHES)  * 100 << "%"<< flush;
				calculateExcident(&patches[p]);	
				if (DEBUG) cout << "Color After Excident Calculation: " << patches[p]._excident->_red << " " << patches[p]._excident->_green << " " << patches[p]._excident->_blue << endl;
			}
			cout << endl;
						
		}
		/*
		for(int p = 0; p < NUMBER_OF_PATCHES; p++)
		{
			checkWhetherPatchHasComponentMax(&patches[p], totalLight);	
		}
		
		printLight(totalLight);		

		for(int p = 0; p < NUMBER_OF_PATCHES; p++)
		{
			//patches[p]._excident->_red /= totalLight->_red;
			//patches[p]._excident->_green /= totalLight->_green;
			//patches[p]._excident->_blue /= totalLight->_blue;

			if (patches[p]._excident->_red != 0.0 || patches[p]._excident->_green != 0.0 || patches[p]._excident->_blue)
			{
				if (DEBUG) cout << patches[p]._excident->_red << " " << patches[p]._excident->_green << " " << patches[p]._excident->_blue << endl;
			}
		}
		*/
}
void renderPatch(patch * p)
{
	// Render bottom	
	light * incident = calculauteIncidentLight(p->_center, p->_normal, p->_up);
	p->_incident->_red = incident->_red;
	p->_incident->_green = incident->_green;
	p->_incident->_blue = incident->_blue;
	if (DEBUG) cout << "Color After Incident Calculation: " << p->_incident->_red << " " << p->_incident->_green << " " << p->_incident->_blue << endl;
	delete incident;
}

void calculateExcident(patch * p)
{
	p->_excident->_red = p->_incident->_red * p->_reflectance->_red + p->_excident->_red;
	p->_excident->_green = p->_incident->_green * p->_reflectance->_green + p->_excident->_green;
	p->_excident->_blue = p->_incident->_blue * p->_reflectance->_blue + p->_excident->_blue;
}

light * calculauteIncidentLight(point * center, Vector * normal, Vector * up)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	int totalPixels = WINDOW_HEIGHT * WINDOW_WIDTH - (WINDOW_HEIGHT * WINDOW_WIDTH) / 4;
	
	light * totalLight = new light(0.0, 0.0, 0.0);
	/* 
	 * Render hemicube
	 */
	// Hemicube left frustrum
	Vector * leftDirection = add(center, crossproduct(up, normal));
	renderHemicubeView(center, up, leftDirection, LEFT);
	

	// Hemicube right frustrum
	Vector * rightDirection = add(center, crossproduct(normal, up));
	renderHemicubeView(center, up, rightDirection, RIGHT);
	

	// Hemicube top frustrum
	Vector * topDirection = add(center, up);
	Vector * reverseNormal = new Vector(-normal->_x, -normal->_y, -normal->_z);
	renderHemicubeView(center, reverseNormal, topDirection, TOP);
	

	// Hemicube bottom frustrum
	Vector * bottomDirection = add(center, crossproduct(leftDirection, normal));
	renderHemicubeView(center, normal, bottomDirection, BOTTOM);

	// Hemicube front 
	Vector * forwardDirection = add(center, normal);
	renderHemicubeView(center, up, forwardDirection, FRONT);

	hemicube * h = new hemicube();
	h->_left = getHemicubePixels(LEFT);
	h->_right = getHemicubePixels(RIGHT);
	h->_top = getHemicubePixels(TOP);
	h->_bottom = getHemicubePixels(BOTTOM);
	h->_front = getHemicubePixels(FRONT);
	hemicube * result = applyMultiplierHemicube(h);
	
	light * topView = getTotalLightOfView(result, TOP);
	light * bottomView = getTotalLightOfView(result, BOTTOM);
	light * leftView = getTotalLightOfView(result, LEFT);
	light * rightView = getTotalLightOfView(result, RIGHT);
	light * frontView = getTotalLightOfView(result, FRONT);
	

	totalLight->_red 	+= topView->_red;
	totalLight->_green 	+= topView->_green;
	totalLight->_blue 	+= topView->_blue;

	totalLight->_red 	+= bottomView->_red;
	totalLight->_green 	+= bottomView->_green;
	totalLight->_blue 	+= bottomView->_blue;

	totalLight->_red 	+= leftView->_red;
	totalLight->_green 	+= leftView->_green;
	totalLight->_blue 	+= leftView->_blue;

	totalLight->_red 	+= rightView->_red;
	totalLight->_green 	+= rightView->_green;
	totalLight->_blue 	+= rightView->_blue;

	totalLight->_red 	+= frontView->_red;
	totalLight->_green 	+= frontView->_green;
	totalLight->_blue 	+= frontView->_blue;

	totalLight->_red /= totalPixels;
	totalLight->_green /= totalPixels;
	totalLight->_blue /= totalPixels;
	
	delete h;
	delete topView;
	delete bottomView;
	delete leftView;
	delete rightView;
	delete frontView;
	return totalLight;
}

light * getTotalLightOfView(hemicube * h, HEMICUBE_VIEW view)
{
	light * totalLightOfView = new light(0.0, 0.0, 0.0);
	float WIDTH, HEIGHT, pixelX, pixelY;
	const char * viewName;
	unsigned char * viewBuffer;
	switch(view)
	{
		case FRONT:
			viewName = "FRONT";
			WIDTH = WINDOW_WIDTH / 2;
			HEIGHT = WINDOW_HEIGHT / 2;
			viewBuffer = h->_front;
			break;
		case TOP:
			viewName = "TOP";
			WIDTH = WINDOW_WIDTH / 2;
			HEIGHT = WINDOW_HEIGHT / 4;
			viewBuffer = h->_top;
			break;
		case LEFT:
			viewName = "LEFT";
			WIDTH = WINDOW_WIDTH / 4;
			HEIGHT = WINDOW_HEIGHT / 2;
			viewBuffer = h->_left;
			break;
		case RIGHT:
			viewName = "RIGHT";
			WIDTH = WINDOW_WIDTH / 4;
			HEIGHT = WINDOW_HEIGHT / 2;
			viewBuffer = h->_right;
			break;
		case BOTTOM:
			viewName = "BOTTOM";
			WIDTH = WINDOW_WIDTH / 2;
			HEIGHT = WINDOW_HEIGHT / 4;
			viewBuffer = h->_bottom;
			break;
	}

	for(int i = 0; i < WIDTH * HEIGHT * 3; i+=3)
	{
		pixelX++;

		totalLightOfView->_red += (int)viewBuffer[i] / 255.0;
		
		totalLightOfView->_green += (int)viewBuffer[i + 1] / 255.0;
		
		totalLightOfView->_blue += (int)viewBuffer[i + 2] / 255.0;

		if (pixelX == WIDTH)
		{
			pixelX = 0;
			pixelY++;
		}
	}
	//delete viewName;
	return totalLightOfView;
}

void checkWhetherPatchHasComponentMax(patch * p, light * totalLight)
{
	if (p->_excident->_red > totalLight->_red )
	{
		totalLight->_red = p->_excident->_red;
	}

	if (p->_excident->_green > totalLight->_green )
	{
		totalLight->_green = p->_excident->_green;
	}	

	if (p->_excident->_blue > totalLight->_blue )
	{
		totalLight->_blue = p->_excident->_blue;
	}
}

void add(light * a, light * b)
{
	a->_red += b->_red;
	a->_green += b->_green;
	a->_blue += b->_blue;
}

void printLight(light * l)
{
	cout << l->_red << " " << l->_green << " " << l->_blue << "\n";
}


void renderFunction()
{
	if(running)
	{
		radiosity();
		running = false;
	}
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, 1.0, 1.0, 5.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	renderScene();
	glFlush();
}

// TODO : should renderScene get passed patches * as a parameter?

void renderScene()
{
	for(int p = 0; p < NUMBER_OF_PATCHES; p++)
	{
		glColor3f(patches[p]._excident->_red, patches[p]._excident->_green, patches[p]._excident->_blue);
		
		if (DEBUG) cout << "Color : " << patches[p]._excident->_red << " " << patches[p]._excident->_green << " " << patches[p]._excident->_blue << endl;
		glBegin(GL_POLYGON);
		for(int v = 0; v < patches[p].numVerts; v++)
		{
			if (DEBUG) cout << patches[p]._vertices[v]._x << " " << patches[p]._vertices[v]._y << " " << patches[p]._vertices[v]._z << endl;
			glVertex3f(patches[p]._vertices[v]._x, patches[p]._vertices[v]._y, patches[p]._vertices[v]._z);
		}
		glEnd();
	}
}

void renderHemicubeView(point * center, Vector * up, Vector * direction, HEMICUBE_VIEW view)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	switch(view)
	{
		case FRONT:
			glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 5.0);
			break;
		case TOP:
			glFrustum(-1.0, 1.0, -1.0, 0.0, 1.0, 5.0);
			break;
		case LEFT:	
			glFrustum(0.0, 1.0, -1.0, 1.0, 1.0, 5.0);
			break;
		case RIGHT:
			glFrustum(-1.0, 0.0, -1.0, 1.0, 1.0, 5.0);
			break;
		case BOTTOM:
			glFrustum(-1.0, 1.0, 0.0, 1.0, 1.0, 5.0);
			break;
	}
	
	glMatrixMode(GL_MODELVIEW);

	switch(view)
	{
		case FRONT:
			glViewport(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
			break;
		case TOP:
			glViewport(WINDOW_WIDTH / 4, WINDOW_HEIGHT * 3 / 4, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 4);
			break;
		case LEFT:
			glViewport(0, WINDOW_HEIGHT * 1 / 4, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2);
			break;
		case RIGHT:
			glViewport(WINDOW_WIDTH * 3 / 4, WINDOW_HEIGHT * 1 / 4, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2);
			break;
		case BOTTOM:
			glViewport(WINDOW_WIDTH / 4, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 4);
			break;
	}

	glLoadIdentity();
	gluLookAt(center->_x, center->_y, center->_z, direction->_x, direction->_y, direction->_z, up->_x, up->_y, up->_z); 
	renderScene();
}

bool blah = true;
unsigned char * applyMultiplier(unsigned char * buffer, HEMICUBE_VIEW view)
{
	float 	WIDTH, 
		HEIGHT, 
		VIEW_X, 
		VIEW_Y, 
		PIXEL_HEIGHT_IN_DEGREES, 
		PIXEL_WIDTH_IN_DEGREES, 
		START_ANGLE_PX_CAMERA_X, 
		START_ANGLE_PX_CAMERA_Y, 
		START_ANGLE_PX_PATCH_X, 
		START_ANGLE_PX_PATCH_Y;
	const char * viewName;
	switch(view)
	{
		case FRONT:
			viewName = "FRONT";
			WIDTH = WINDOW_WIDTH / 2;
			HEIGHT = WINDOW_HEIGHT / 2;
			VIEW_X = WINDOW_WIDTH / 4;
			VIEW_Y = WINDOW_HEIGHT / 4;
			PIXEL_HEIGHT_IN_DEGREES = 45.0 / (HEIGHT / 2.0);
			PIXEL_WIDTH_IN_DEGREES = 45.0 / (WIDTH / 2.0);
			START_ANGLE_PX_CAMERA_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_CAMERA_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			break;
		case TOP:
			viewName = "TOP";
			WIDTH = WINDOW_WIDTH / 2;
			HEIGHT = WINDOW_HEIGHT / 4;
			VIEW_X = WINDOW_WIDTH / 4;
			VIEW_Y = WINDOW_HEIGHT * 3 / 4;
			PIXEL_HEIGHT_IN_DEGREES = 45.0 / HEIGHT;
			PIXEL_WIDTH_IN_DEGREES = 45.0 / (WIDTH / 2.0);
			START_ANGLE_PX_CAMERA_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_CAMERA_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_Y = 45.0 + PIXEL_HEIGHT_IN_DEGREES / 2.0;
			break;
		case LEFT:
			viewName = "LEFT";
			WIDTH = WINDOW_WIDTH / 4;
			HEIGHT = WINDOW_HEIGHT / 2;
			VIEW_X = 0;
			VIEW_Y = WINDOW_HEIGHT * 1 / 4;
			PIXEL_HEIGHT_IN_DEGREES = 45.0 / (HEIGHT / 2.0);
			PIXEL_WIDTH_IN_DEGREES = 45.0 / WIDTH;
			START_ANGLE_PX_CAMERA_X = PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_CAMERA_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_X = 90.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			break;
		case RIGHT:
			viewName = "RIGHT";
			WIDTH = WINDOW_WIDTH / 4;
			HEIGHT = WINDOW_HEIGHT / 2;
			VIEW_X = WINDOW_WIDTH * 3 / 4;
			VIEW_Y = WINDOW_HEIGHT * 1 / 4;
			PIXEL_HEIGHT_IN_DEGREES = 45.0 / (HEIGHT / 2.0);
			PIXEL_WIDTH_IN_DEGREES = 45.0 / WIDTH;
			START_ANGLE_PX_CAMERA_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_CAMERA_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_X = 45.0 + PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			break;
		case BOTTOM:
			viewName = "BOTTOM";
			WIDTH = WINDOW_WIDTH / 2;
			HEIGHT = WINDOW_HEIGHT / 4;
			VIEW_X = WINDOW_WIDTH / 4;
			VIEW_Y = 0;
			PIXEL_HEIGHT_IN_DEGREES = 45.0 / HEIGHT;
			PIXEL_WIDTH_IN_DEGREES = 45.0 / (WIDTH / 2.0);
			START_ANGLE_PX_CAMERA_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_CAMERA_Y = PIXEL_HEIGHT_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_Y = 90.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			break;
	}

	/* 
	 * Apply "Mulitiplier Hemicube"
	 */
	float yawPxCamera = START_ANGLE_PX_CAMERA_X;	// Yaw (in degrees) between the current pixel's center and the direction of the camera
	float pitchPxCamera = START_ANGLE_PX_CAMERA_Y;	// Pitch (in degrees) between the current pixel's center and the direction of the camera

	float yawPxPatch = START_ANGLE_PX_PATCH_X;		// Yaw (in degrees) between the current pixel's center and the normal of the patch
	float pitchPxPatch = START_ANGLE_PX_PATCH_Y;		// Pitch (in degrees) between the current pixel's center and the normal of the patch

	float pixelX = 0;
	float pixelY = 0;
	float twoPi = 2.0 * 3.14159265359;
	for(int i = 0; i < WIDTH * HEIGHT * 3; i+=3)
	{	
		float perspectiveCompensationX = (yawPxCamera / 360.0) * twoPi;
		float perspectiveCompensationY = (pitchPxCamera / 360.0) * twoPi;

		float lambertCompensationX = (yawPxPatch / 360.0) * twoPi;
		float lambertCompensationY = (pitchPxPatch / 360.0) * twoPi;
		 
		float perspectiveCompnesation = cos(perspectiveCompensationX) * cos(perspectiveCompensationY);
		float lambertCompensation = cos(lambertCompensationX) * cos(lambertCompensationY);
		
		// TODO : unit conversion from 0 - 255 to 0 - 1 (?)
		float redComponent = (int)buffer[i] * perspectiveCompnesation * lambertCompensation;
		float greenComponent = (int)buffer[i + 1] * perspectiveCompnesation * lambertCompensation;
		float blueComponent = (int)buffer[i + 2] * perspectiveCompnesation * lambertCompensation;
		
		buffer[i] = redComponent;
		buffer[i + 1] = greenComponent;
		buffer[i + 2] = blueComponent;

		yawPxCamera -= PIXEL_WIDTH_IN_DEGREES;
		switch(view)
		{
			case RIGHT:
				yawPxPatch += PIXEL_WIDTH_IN_DEGREES;
				break;
			default:
				yawPxPatch -= PIXEL_WIDTH_IN_DEGREES;
				break;
		}

		pixelX++;

		if (pixelX == WIDTH)
		{
			pixelX = 0;
			pixelY++;
			yawPxCamera = START_ANGLE_PX_CAMERA_X;
			yawPxPatch = START_ANGLE_PX_PATCH_X;
			pitchPxCamera -= PIXEL_HEIGHT_IN_DEGREES;
			switch(view)
			{
				case FRONT:
					pitchPxPatch -= PIXEL_HEIGHT_IN_DEGREES;
					break;
				case LEFT:
					pitchPxPatch -= PIXEL_HEIGHT_IN_DEGREES;
					break;
				case RIGHT:
					pitchPxPatch -= PIXEL_HEIGHT_IN_DEGREES;
					break;
				case BOTTOM:
					pitchPxPatch -= PIXEL_HEIGHT_IN_DEGREES;
					break;
				case TOP:
					pitchPxPatch += PIXEL_WIDTH_IN_DEGREES;
					break;
			}
		}
	}
	
	//png.close();
	return buffer; 
}
hemicube * applyMultiplierHemicube(hemicube * h)
{
	hemicube * result = new hemicube();
	result->_left = applyMultiplier(h->_left, LEFT);
	result->_right = applyMultiplier(h->_right, RIGHT);
	result->_top = applyMultiplier(h->_top, TOP);
	result->_bottom = applyMultiplier(h->_bottom, BOTTOM);
	result->_front = applyMultiplier(h->_front, FRONT);

	return result;
}
unsigned char * getHemicubePixels(HEMICUBE_VIEW view)
{
	float 	WIDTH, 
		HEIGHT, 
		VIEW_X, 
		VIEW_Y, 
		PIXEL_HEIGHT_IN_DEGREES, 
		PIXEL_WIDTH_IN_DEGREES, 
		START_ANGLE_PX_CAMERA_X, 
		START_ANGLE_PX_CAMERA_Y, 
		START_ANGLE_PX_PATCH_X, 
		START_ANGLE_PX_PATCH_Y;
	const char * viewName;
	unsigned char * buffer;
	switch(view)
	{
		case FRONT:
			viewName = "FRONT";
			WIDTH = WINDOW_WIDTH / 2;
			HEIGHT = WINDOW_HEIGHT / 2;
			VIEW_X = WINDOW_WIDTH / 4;
			VIEW_Y = WINDOW_HEIGHT / 4;
			PIXEL_HEIGHT_IN_DEGREES = 45.0 / (HEIGHT / 2.0);
			PIXEL_WIDTH_IN_DEGREES = 45.0 / (WIDTH / 2.0);
			START_ANGLE_PX_CAMERA_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_CAMERA_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			break;
		case TOP:
			viewName = "TOP";
			WIDTH = WINDOW_WIDTH / 2;
			HEIGHT = WINDOW_HEIGHT / 4;
			VIEW_X = WINDOW_WIDTH / 4;
			VIEW_Y = WINDOW_HEIGHT * 3 / 4;
			PIXEL_HEIGHT_IN_DEGREES = 45.0 / HEIGHT;
			PIXEL_WIDTH_IN_DEGREES = 45.0 / (WIDTH / 2.0);
			START_ANGLE_PX_CAMERA_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_CAMERA_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_Y = 45.0 + PIXEL_HEIGHT_IN_DEGREES / 2.0;
			break;
		case LEFT:
			viewName = "LEFT";
			WIDTH = WINDOW_WIDTH / 4;
			HEIGHT = WINDOW_HEIGHT / 2;
			VIEW_X = 0;
			VIEW_Y = WINDOW_HEIGHT * 1 / 4;
			PIXEL_HEIGHT_IN_DEGREES = 45.0 / (HEIGHT / 2.0);
			PIXEL_WIDTH_IN_DEGREES = 45.0 / WIDTH;
			START_ANGLE_PX_CAMERA_X = PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_CAMERA_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_X = 90.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			break;
		case RIGHT:
			viewName = "RIGHT";
			WIDTH = WINDOW_WIDTH / 4;
			HEIGHT = WINDOW_HEIGHT / 2;
			VIEW_X = WINDOW_WIDTH * 3 / 4;
			VIEW_Y = WINDOW_HEIGHT * 1 / 4;
			PIXEL_HEIGHT_IN_DEGREES = 45.0 / (HEIGHT / 2.0);
			PIXEL_WIDTH_IN_DEGREES = 45.0 / WIDTH;
			START_ANGLE_PX_CAMERA_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_CAMERA_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_X = 45.0 + PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_Y = 45.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			break;
		case BOTTOM:
			viewName = "BOTTOM";
			WIDTH = WINDOW_WIDTH / 2;
			HEIGHT = WINDOW_HEIGHT / 4;
			VIEW_X = WINDOW_WIDTH / 4;
			VIEW_Y = 0;
			PIXEL_HEIGHT_IN_DEGREES = 45.0 / HEIGHT;
			PIXEL_WIDTH_IN_DEGREES = 45.0 / (WIDTH / 2.0);
			START_ANGLE_PX_CAMERA_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_CAMERA_Y = PIXEL_HEIGHT_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_X = 45.0 - PIXEL_WIDTH_IN_DEGREES / 2.0;
			START_ANGLE_PX_PATCH_Y = 90.0 - PIXEL_HEIGHT_IN_DEGREES / 2.0;
			break;
	}
	buffer = (unsigned char *)malloc(WIDTH * HEIGHT * 3);

	glReadPixels(VIEW_X, VIEW_Y, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	return buffer; 
}


void initializePatches()
{
	if(DEBUG) cout << "Generating patches from obj file... \n";
	skyPatch->_emission = new light(1.0, 1.0, 1.0);

	skyPatch->_excident = skyPatch->_emission;
	bottomPatch->_excident = bottomPatch->_emission;
	leftPatch->_excident = leftPatch->_emission;
	rightPatch->_excident = rightPatch->_emission;
	backPatch->_excident = backPatch->_emission;

	patches = new patch[shapes.size()];
	
	NUMBER_OF_PATCHES = shapes.size();
	int verts = 0;	
	for (size_t i = 0; i < shapes.size(); i++)
	{	
		int numberVertices = shapes[i].mesh.positions.size() / 3;
		patches[i]._vertices = new point[numberVertices];
		if(DEBUG) cout << "Patch " << shapes[i].name.c_str() << endl;
		for(size_t v = 0; v < numberVertices; v++)
		{
			verts++;
			/**/
			float x = shapes[i].mesh.positions[3*v+0];
			float y = shapes[i].mesh.positions[3*v+1];
			float z = shapes[i].mesh.positions[3*v+2];
			
			patches[i]._vertices[v]._x = x;
			patches[i]._vertices[v]._y = y;
			patches[i]._vertices[v]._z = z;
			if(DEBUG) cout << verts << " " << patches[i]._vertices[v]._x << " " << patches[i]._vertices[v]._y << " " << patches[i]._vertices[v]._z << endl;
			
		}
	
		float emission_red = shapes[i].material.emission[0];
		float emission_green = shapes[i].material.emission[1];
		float emission_blue = shapes[i].material.emission[2];

		float diffuse_red = shapes[i].material.diffuse[0];
		float diffuse_green = shapes[i].material.diffuse[1];
		float diffuse_blue = shapes[i].material.diffuse[2];
		
		patches[i]._emission->_red = emission_red;
		patches[i]._emission->_green = emission_green;
		patches[i]._emission->_blue = emission_blue;

		patches[i]._reflectance->_red = diffuse_red;
		patches[i]._reflectance->_green = diffuse_green;
		patches[i]._reflectance->_blue = diffuse_blue;

		patches[i]._excident->_red = patches[i]._emission->_red;
		patches[i]._excident->_green = patches[i]._emission->_green;
		patches[i]._excident->_blue = patches[i]._emission->_blue;

		if(DEBUG) cout << "Emission    : " << patches[i]._emission->_red << " " << patches[i]._emission->_green << " " << patches[i]._emission->_blue << endl;
		if(DEBUG) cout << "Reflectance : " << patches[i]._reflectance->_red << " " << patches[i]._reflectance->_green << " " << patches[i]._reflectance->_blue << endl;
		
		// Calculate normal
		Vector * a = getVector(&patches[i]._vertices[0], &patches[i]._vertices[1]);
		Vector * b = getVector(&patches[i]._vertices[1], &patches[i]._vertices[2]);
		
		patches[i]._normal = crossproduct(a, b);
		normalize(patches[i]._normal);

		if(DEBUG) cout << "Normal     : " << patches[i]._normal->_x << " " << patches[i]._normal->_y << " " << patches[i]._normal->_z << endl;

		// Calculate up vector
		Vector * right = crossproduct(patches[i]._normal, new Vector(0.0, 1.0, 0.0));
		
		Vector * upVector = crossproduct(right, patches[i]._normal);

		patches[i]._up = upVector;

		if(patches[i]._up->_x == 0.0 && patches[i]._up->_y == 0.0 && patches[i]._up->_z == 0.0)
		{
			patches[i]._up->_x = 0;
			patches[i]._up->_y = 0;
			patches[i]._up->_z = -1;
		}
		
		if(DEBUG) cout << "Up        : " << patches[i]._up->_x << " " << patches[i]._up->_y << " " << patches[i]._up->_z << endl;

		// Calculate center point
		float centerX = 0;
		float centerY = 0;
		float centerZ = 0;			
		for(int v = 0; v < numberVertices; v++)
		{
			centerX += patches[i]._vertices[v]._x;
			centerY += patches[i]._vertices[v]._y;
			centerZ += patches[i]._vertices[v]._z;
		}

		patches[i].numVerts = numberVertices;


		centerX /= numberVertices;
		centerY /= numberVertices;
		centerZ /= numberVertices;

		patches[i]._center = new point(centerX, centerY, centerZ);

		if(DEBUG) cout << "Center        : " << patches[i]._center->_x << " " << patches[i]._center->_y << " " << patches[i]._center->_z << endl;

		/*
		delete right;
		delete upVector;
		delete a;
		delete b;
		*/
	}
	if(DEBUG) cout << NUMBER_OF_PATCHES << " patches" << endl;
		
}

int main(int argc, char* argv[])
{
	
	string inputfile = argv[1];
	ITERATIONS = atoi(argv[2]);
	if(atoi(argv[3]) == 1) DEBUG = true;

	string err = tinyobj::LoadObj(shapes, inputfile.c_str());

	if(!err.empty())
	{
		cerr << err << endl;
		exit(1);
	}

	cout << "Running radiosity on " << inputfile << " with " << ITERATIONS << " iterations.\n";

	initializePatches();
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
	glutInitWindowPosition(100,100);
    	glutCreateWindow("OpenGL - First window demo");
	glutDisplayFunc(renderFunction);
	glutMainLoop();
	/**/
	return 0;
}

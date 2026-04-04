// Copyright (C) 2025 Jiri Fajta
// License: MIT license

// Snipped code:
// This is not complete code, but it provides an idea how to implement.

typedef struct {
	Camera camera[2];// Left and right camera (custom struct).
	short cameraOffsetCenter;// Strength of 3D effect can be adjusted here. World coordinate system distance.
	short num_cameras;// is 2.
} CameraStereo;

// init:
// variable "displayData" is part of custom render engine.
// "displayData->displayBuffer[i].drawSplitscreen" has 2x DRAWENV for each i-th where i=[0,1].
CameraStereo camera;
CameraStereo *cam = &camera;

//PROTOTYPE
void run();
void update();
void render();

void run(){
	//Init cameras
	cam->cameraOffsetCenter = 384;// Offset between camera and center of two camera. 
	cam->num_cameras = 2;
	//CameraSet(Camera *cam, posX, posY, posZ, rotX, rotY, rotZ);
	CameraSet(&cam->camera[0],  cam->cameraOffsetCenter,0,0,0,0,0);
	CameraSet(&cam->camera[1], -cam->cameraOffsetCenter,0,0,0,0,0);
	init_display();
	while(1){
		update();//update mesh and all cameras.
		render();
	}
}

void render(){
	int draw_i;
	for (draw_i = 0; draw_i < cam->num_cameras; draw_i++) {
			//wait till DrawOTag(...) is finished.
			DrawSync(0);
			// Clear ordering table, this was used for
			ClearOTag(displayData->displayBuffer[displayData->dbi].ot, displayData->OTLEN );
			//set background color
			setRGB0( (displayData->displayBuffer[0].drawSplitscreen + draw_i) , 96,96, 250 );
			setRGB0( (displayData->displayBuffer[1].drawSplitscreen + draw_i) , 96,96, 250 );
			// set camera matrix of camera 1 or 2.
			Camera_draw(&cam->camera[draw_i]);
			//SET GTE MATRIX: transform 3D object local space to world space.
			toGteVec(&cam->camera[draw_i], &mesh_position, &mesh_rotation);// needs to be done per mesh.
			// Generate primitive polygons from 3D mesh transformed to screen space.
			generate_primitives(&mesh);
			//set draw buffer to use
		    PutDrawEnv(displayData->displayBuffer[displayData->dbi].drawSplitscreen + draw_i);
			// draw primitive to draw buffer
		    DrawOTag(displayData->displayBuffer[displayData->dbi].ot );
	}//end for
	// swap display-buffer.
	PutDispEnv(&displayData->displayBuffer[!displayData->dbi].disp);// Display 2 draw-buffer next to each other at once (upper 2 draw-buffers or bottem 2).
	displayData->dbi ^= 1;// which of each pair of draw-buffer is active do write primitives to (upper 2 draw-buffers or bottem 2).
	ClearOTag(displayData->displayBuffer[displayData->dbi].ot, displayData->OTLEN);
}
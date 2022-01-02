#define is_down(b)  input->buttons[b].is_down
#define pressed(b)  (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

class Engine3D {
private:

	bool debug_wireframe = false; // Render mesh wireframe
	bool debug_spinning = false;  // Makes the world spin. use with a small obj file

	float fTheta = 0.f;
	float fYaw = 3.f;
	float fPitch = 0.f;
	float fSpeed = 20.f;
	float fRotationSpeed = .2f;

	float fFov = 75.f;
	float fNear = 0.1f;
	float fFar = 1000.f;

	mat4x4 matProj;
	vec3d vCamera;
	vec3d vLookDir;

	mesh meshCube;
	Bitmap* sprTex1;

public:
	void game_init() {

		pDepthBuffer = new float[render_state.height * render_state.width];

		if (debug_spinning) {
			meshCube.LoadFromObjectFile("model\\ship.obj");
			sprTex1 = new Bitmap("model\\ship.bmp");
		} else {
			meshCube.LoadFromObjectFile("model\\Artisans Hub.obj");
			sprTex1 = new Bitmap("model\\Artisans Hub.bmp");
		}

		// Projection Matrix
		matProj = Matrix_MakeProjection(fFov, (float)render_state.height / (float)render_state.width, fNear, fFar);
	}

	void game_update(Input* input, double fElapsedTime) {

		vec3d vRight = vLookDir.cross(vec3d_up);

		if (is_down(BUTTON_SPACE))
			vCamera = vCamera + vec3d_up * fSpeed * (float)fElapsedTime;
		if (is_down(BUTTON_CONTROL))
			vCamera = vCamera - vec3d_up * fSpeed * (float)fElapsedTime;

		if (is_down(BUTTON_W))
			vCamera = vCamera + vLookDir * fSpeed * (float)fElapsedTime;
		if (is_down(BUTTON_S))
			vCamera = vCamera - vLookDir * fSpeed * (float)fElapsedTime;
		if (is_down(BUTTON_A))
			vCamera = vCamera - vRight * fSpeed * (float)fElapsedTime;
		if (is_down(BUTTON_D))
			vCamera = vCamera + vRight * fSpeed * (float)fElapsedTime;

		if (is_down(BUTTON_UP))
			fPitch += fRotationSpeed * fSpeed * (float)fElapsedTime;
		if (is_down(BUTTON_DOWN))
			fPitch -= fRotationSpeed * fSpeed * (float)fElapsedTime;
		if (is_down(BUTTON_LEFT))
			fYaw -= fRotationSpeed * fSpeed * (float)fElapsedTime;
		if (is_down(BUTTON_RIGHT))
			fYaw += fRotationSpeed * fSpeed * (float)fElapsedTime;

		fPitch = clamp(-1.5f, fPitch, 1.5f);

		mat4x4 matTrans;
		matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 5.0f);

		mat4x4 matWorld;
		matWorld.makeIdentity(); // Form World Matrix

		// These can be used to make a spinning object for debugging
		if (debug_spinning) {
			// Set up rotation matrices
			mat4x4 matRotX, matRotY, matRotZ;

			fTheta += 1.0f * (float)fElapsedTime;

			matRotX = Matrix_MakeRotationX(0);// fTheta);
			matRotY = Matrix_MakeRotationY(fTheta * .25f);
			matRotZ = Matrix_MakeRotationZ(0);// fTheta * .5f);

			matWorld = matRotX * matRotY; // Transform by rotation
			matWorld *= matTrans;		  // Transform by translation
		} else {
			matWorld = matTrans;
		}

		// The view space is a Right-handed system, where the X-axis points to the right and the Y-axis points up.
		// The Z-axis points out of the view. hence (vUp = vec3d_down) & (vTarget = vec3d_back)
		vec3d vUp = vec3d_down; vec3d vTarget = vec3d_back;

		mat4x4 matCameraRotX = Matrix_MakeRotationX(fPitch);
		mat4x4 matCameraRotY = Matrix_MakeRotationY(fYaw);
		vLookDir = matCameraRotX * matCameraRotY * vTarget;

		vTarget = vCamera + vLookDir;
		mat4x4 matCamera = Matrix_PointAt(vCamera, vTarget, vUp);

		// View matrix is the inverse of the camera matrix
		mat4x4 matView = matCamera.inverse();

		// Store triagles for rastering later
		vector<triangle> vecTrianglesToRaster;

		// Draw Triangles
		for (auto tri : meshCube.tris) {
			triangle triProjected, triTransformed, triViewed;

			// World Matrix Transform
			triTransformed.p[0] = matWorld * tri.p[0];
			triTransformed.p[1] = matWorld * tri.p[1];
			triTransformed.p[2] = matWorld * tri.p[2];
			triTransformed.t[0] = tri.t[0];
			triTransformed.t[1] = tri.t[1];
			triTransformed.t[2] = tri.t[2];
			
			// To update normals, multiply by the inverse of the transpose of the world matrix.
			// normals are the same for each point so only do this once.
			vec3d normTransformed = matWorld.transpose().inverse() * tri.n[0];
			
			// Get Ray from triangle to camera
			vec3d vCameraRay = triTransformed.p[0] - vCamera;

			// If ray is aligned with normal, then triangle is visible
			if (normTransformed.dot(vCameraRay) < 0.0f)
			{
				// Convert World Space --> View Space
				triViewed.p[0] = matView * triTransformed.p[0];
				triViewed.p[1] = matView * triTransformed.p[1];
				triViewed.p[2] = matView * triTransformed.p[2];
				triViewed.t[0] = triTransformed.t[0];
				triViewed.t[1] = triTransformed.t[1];
				triViewed.t[2] = triTransformed.t[2];

				// Clip Viewed Triangle against near plane, this could form two additional triangles. 
				int nClippedTriangles = 0;
				triangle clipped[2];
				nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				for (int n = 0; n < nClippedTriangles; n++) {
					// Project triangles from 3D --> 2D
					triProjected.p[0] = matProj * clipped[n].p[0];
					triProjected.p[1] = matProj * clipped[n].p[1];
					triProjected.p[2] = matProj * clipped[n].p[2];
					triProjected.t[0] = clipped[n].t[0];
					triProjected.t[1] = clipped[n].t[1];
					triProjected.t[2] = clipped[n].t[2];

					triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
					triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
					triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

					triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
					triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
					triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;

					triProjected.t[0].w = 1.0f / triProjected.p[0].w;
					triProjected.t[1].w = 1.0f / triProjected.p[1].w;
					triProjected.t[2].w = 1.0f / triProjected.p[2].w;

					// Scale into view
					triProjected.p[0] = triProjected.p[0] / triProjected.p[0].w;
					triProjected.p[1] = triProjected.p[1] / triProjected.p[1].w;
					triProjected.p[2] = triProjected.p[2] / triProjected.p[2].w;

					// Offset verts into visible normalised space
					vec3d vOffsetView = { 1,1,0 };
					triProjected.p[0] = triProjected.p[0] + vOffsetView;
					triProjected.p[1] = triProjected.p[1] + vOffsetView;
					triProjected.p[2] = triProjected.p[2] + vOffsetView;
					triProjected.p[0].x *= .5f * (float)render_state.width;
					triProjected.p[0].y *= .5f * (float)render_state.height;
					triProjected.p[1].x *= .5f * (float)render_state.width;
					triProjected.p[1].y *= .5f * (float)render_state.height;
					triProjected.p[2].x *= .5f * (float)render_state.width;
					triProjected.p[2].y *= .5f * (float)render_state.height;

					// Store triangle for sorting
					vecTrianglesToRaster.push_back(triProjected);
				}
			}
		}
		
		// Clear Screen
		clear_screen(COLOR_BLACK);

		// Clear Depth Buffer
		for (int i = 0; i < render_state.width * render_state.height; i++) {
			pDepthBuffer[i] = 0.f;
		}

		for (auto& triToRaster : vecTrianglesToRaster) {
			// Clip triangles against all four screen edges, this could yield a bunch of triangles
			triangle clipped[2];
			list<triangle> listTriangles;
			listTriangles.push_back(triToRaster);
			int nNewTriangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					// Clip it against a plane. We only need to test each subsequent plane, against subsequent new triangles
					// as all triangles after a plane clip are guaranteed to lie on the inside of the plane.
					switch (p)
					{
						case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
						case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)render_state.height - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
						case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
						case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)render_state.width - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					}

					// Clipping may yield a variable number of triangles, so add these new ones to the back 
					// of the queue for subsequent clipping against next planes
					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				nNewTriangles = (int)listTriangles.size();
			}

			// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
			for (auto& t : listTriangles)
			{
				texture_triangle_in_pixels(t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
										   t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
										   t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w, sprTex1);

				if (debug_wireframe) {
					draw_triangle_in_pixels((int)t.p[0].x, (int)t.p[0].y,
											(int)t.p[1].x, (int)t.p[1].y,
											(int)t.p[2].x, (int)t.p[2].y, COLOR_WHITE);
				}
			}
		}
	}
};
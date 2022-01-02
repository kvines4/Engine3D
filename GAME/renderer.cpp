//	Drawn with top-down DIB with the origin at the upper left corner
//
//	Up    = -Negative y
//	Down  = +Positive y
//	Left  = -Negative x
//	Right = +Positive x
// 
//	0,0 ────────> 5,0
//	 | ╔═╦═╦═╦═╦═╗ |
//	 | ╠═╬═╬═╬═╬═╣ |
//	 | ╠═╬═╬═╬═╬═╣ |
//	 | ╠═╬═╬═╬═╬═╣ |
//	 | ╠═╬═╬═╬═╬═╣ |
//	 V ╚═╩═╩═╩═╩═╝ V
//  0,5 ────────> 5,5

global_variable double render_scale = 0.005f;

internal int
get_render_scale() {
	return (render_state.height < render_state.width)
		? (int)(render_state.height * render_scale)
		: (int)(render_state.width * render_scale);
}

internal void
scale_coordinates(int* x, int* y) {
	int scale = get_render_scale();
	*x *= scale;
	*y *= scale;
}

internal void
scale_coordinates(int* x, int* y, int* z) {
	int scale = get_render_scale();
	*x *= scale;
	*y *= scale;
	*z *= scale;
}

internal void
scale_coordinates(int* x0, int* y0, int* x1, int* y1) {
	int scale = get_render_scale();
	*x0 *= scale;
	*y0 *= scale;
	*x1 *= scale;
	*y1 *= scale;
}

internal void
scale_polygon_coordinates(vector<int2>* points) {
	int scale = get_render_scale();
	int length = (int)(*points).size();
	for (int i = 0; i < length; i++) {
		(*points)[i].x *= scale;
		(*points)[i].y *= scale;
	}
}

internal void
clear_screen(u32 color) {
	u32* pixel = (u32*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color;
		}
	}
}

internal void
draw_pixel(int x, int y, u32 color) {
	u32* pixel = (u32*)render_state.memory + x + y * render_state.width;
	*pixel = color;
}

internal void
draw_pixel(int x, int y, Color color) {
	u32* pixel = (u32*)render_state.memory + x + y * render_state.width;
	
	u32 c = color.to_hex();
	
	*pixel = c;
}

internal void
draw_line_in_pixels(int x1, int y1, int x2, int y2, u32 color)
{
	// Uses Bresenham’s Line Generation Algorithm
	// draw left to right
	if (x1 > x2) {
		int temp_x = x1, temp_y = y1;
		x1 = x2;
		y1 = y2;
		x2 = temp_x;
		y2 = temp_y;
	}

    int dx = x2 - x1;
    int dy = y2 - y1;
    int absdx = abs(dx);
    int absdy = abs(dy);

    int x = x1;
    int y = y1;

	if (x >= 0 && x < render_state.width && y >= 0 && y < render_state.height)
		draw_pixel(x, y, color);

    // slope < 1
    if (absdx > absdy) {

        int d = 2 * absdy - absdx;

        for (int i = 1; i <= absdx; i++) {
            x = dx < 0 ? x - 1 : x + 1;
            if (d < 0) {
                d = d + 2 * absdy;
            } else {
                y = dy < 0 ? y - 1 : y + 1;
                d = d + (2 * absdy - 2 * absdx);
            }
			if (x >= 0 && x < render_state.width && y >= 0 && y < render_state.height)
			{
				draw_pixel(x, y, color);
			}
        }
    } else { // case when slope is greater than or equals to 1
        int d = 2 * absdx - absdy;

        for (int i = 1; i <= absdy; i++)
        {
            y = dy < 0 ? y - 1 : y + 1;
            if (d < 0)
                d = d + 2 * absdx;
            else
            {
                x = dx < 0 ? x - 1 : x + 1;
                d = d + (2 * absdx) - (2 * absdy);
            }
			if (x >= 0 && x < render_state.width && y >= 0 && y < render_state.height)
			{
				draw_pixel(x, y, color);
			}
        }
    }
}

internal void
draw_line(int x1, int y1, int x2, int y2, u32 color) {
	scale_coordinates(&x1, &y1, &x2, &y2);
	draw_line_in_pixels(x1, y1, x2, y2, color);
}

internal void
fill_rect_in_pixels(int x0, int y0, int width, int height, u32 color) {

	x0 = clamp(0, x0, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	int x1 = clamp(0, x0 + width, render_state.width);
	int y1 = clamp(0, y0 + height, render_state.height);

	for (int y = y0; y < y1; y++) {
		u32* pixel = (u32*)render_state.memory + x0 + y * render_state.width;
		for (int x = x0; x < x1; x++) {
			*pixel++ = color;
		}
	}
}

internal void
fill_rect(int x, int y, int width, int height, u32 color) {
	scale_coordinates(&x, &y, &width, &height);
	fill_rect_in_pixels(x, y, width, height, color);
}

internal void
draw_rect_in_pixels(int x0, int y0, int width, int height, u32 color) {

	x0 = clamp(0, x0, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	int x1 = clamp(0, x0 + width, render_state.width);
	int y1 = clamp(0, y0 + height, render_state.height);

	u32* pixel;

	// Draw sides
	for (int y = y0; y < y1; y++) {
		if (x0 >= 0 && x0 < render_state.width) {
			pixel = (u32*)render_state.memory + x0 + y * render_state.width;
			*pixel = color;
		}
		if (x1 >= 0 && x1 < render_state.width) {
			pixel = (u32*)render_state.memory + x1 + y * render_state.width - 1;
			*pixel = color;
		}
	}

	// Draw top and bottom
	for (int x = x0; x < x1; x++) {
		if (y0 >= 0 && y0 < render_state.height) {
			pixel = (u32*)render_state.memory + x + y0 * render_state.width;
			*pixel = color;
		}
		if (y1 >= 0 && y1 < render_state.height) {
			u32* pixel = (u32*)render_state.memory + x + (y1 - 1) * render_state.width;
			*pixel = color;
		}
	}
}

internal void
draw_rect(int x, int y, int width, int height, u32 color) {
	scale_coordinates(&x, &y, &width, &height);
	draw_rect_in_pixels(x, y, width, height, color);
}

internal void
fill_ellipse_in_pixels(int x0, int y0, int radius, u32 color) {

	for (int y = -radius; y <= radius; y++)
		for (int x = -radius; x <= radius; x++)
			if (x * x + y * y <= radius * radius) {
				int ax = x0 + x;
				int ay = y0 + y;

				if (ax >= 0 && ax < render_state.width &&
					ay >= 0 && ay < render_state.height)
					draw_pixel(ax, ay, color);
			}
}

internal void
fill_ellipse(int x, int y, int radius, u32 color) {
	scale_coordinates(&x, &y, &radius);
	fill_ellipse_in_pixels(x, y, radius, color);
}

// TODO: draw_ellipse

internal void
draw_polygon_in_pixels(vector<int2> points, u32 color) {
	// Draw a line between each pair of points
	int length = (int)points.size();
	for (int i = 0; i < length-1; i++) {
		draw_line_in_pixels(points[i].x, points[i].y, points[i+1].x, points[i+1].y, color);
	}
	draw_line_in_pixels(points[length - 1].x, points[length - 1].y, points[0].x, points[0].y, color);
}

internal void
draw_polygon(vector<int2> points, u32 color) {
	scale_polygon_coordinates(&points);
	draw_polygon_in_pixels(points, color);
}

internal void
fill_polygon_in_pixels(vector<int2> points, u32 color) {

	int length = (int)points.size();
	vector<int> nodesX;
	int y0 = 0;
	int x_offset = 1; // inclusing offset to line up better with other drawing functions

	int y1 = render_state.height;
	// find top and bottom bounds of the polygon
	for (int i = 0; i < length; i++) {
		if (points[i].y < y0 && points[i].y > 0) y0 = points[i].y;
		if (points[i].y > y1 && points[i].y < render_state.height) y1 = points[i].y;
	}

	// Loop through the rows of the image.
	for (int y = y0; y < y1; y++) {

		// Build a list of nodes that intersect this row
		for (int i = 0, j = length - 1; i < length; i++) {

			if (points[i].y < y && points[j].y >= y || 
				points[j].y < y && points[i].y >= y) 
			{
				// Need to convert to float for this to work or we get big squares lol.
				float ix = (float)points[i].x;
				float iy = (float)points[i].y;
				float jx = (float)points[j].x;
				float jy = (float)points[j].y;

				nodesX.push_back((int)(ix + (y - iy) / (jy - iy) * (jx - ix)) + x_offset);
			}
			j = i;
		}
		int nodeCount = (int)nodesX.size();

		// Sort the nodes
		std::sort(nodesX.begin(), nodesX.end());

		// Fill the pixels between node pairs.
		for (int i = 0; i < nodeCount; i += 2) {
			// Check if row is within frame
			if (nodesX[i    ] > render_state.width) break;
			if (nodesX[i + 1] >  0) {
				// Ensure min max values are within the frame
				if (nodesX[i    ] < 0) 
					nodesX[i    ] = 0;
				if (nodesX[i + 1] > render_state.width)
					nodesX[i + 1] = render_state.width;

				for (int x = nodesX[i]; x < nodesX[i + 1]; x++) {
					draw_pixel(x, y, color);
				}
			}
		}
		nodesX.clear();
	}
}

internal void
fill_polygon(vector<int2> points, u32 color) {
	scale_polygon_coordinates(&points);
	fill_polygon_in_pixels(points, color);
}

internal void
draw_triangle_in_pixels(int x1, int y1, int x2, int y2, int x3, int y3, u32 color) {
	draw_line_in_pixels(x1, y1, x2, y2, color);
	draw_line_in_pixels(x2, y2, x3, y3, color);
	draw_line_in_pixels(x3, y3, x1, y1, color);
}

internal void
draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, u32 color) {
	scale_coordinates(&x1, &y1);
	scale_coordinates(&x2, &y2);
	scale_coordinates(&x3, &y3);
	draw_triangle_in_pixels(x1, y1, x2, y2, x3, y3, color);
}

internal void
fill_triangle_in_pixels(int x1, int y1, int x2, int y2, int x3, int y3, u32 color) {
	vector<int2> points;

	points.push_back({ x1, y1 });
	points.push_back({ x2, y2 });
	points.push_back({ x3, y3 });

	fill_polygon_in_pixels(points, color);
}

internal void
fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, u32 color) {
	scale_coordinates(&x1, &y1);
	scale_coordinates(&x2, &y2);
	scale_coordinates(&x3, &y3);
	fill_triangle_in_pixels(x1, y1, x2, y2, x3, y3, color);
}

internal void
texture_triangle_in_pixels(int x1, int y1, float u1, float v1, float w1,
						   int x2, int y2, float u2, float v2, float w2,
						   int x3, int y3, float u3, float v3, float w3,
						   Bitmap* tex)
{
	if (y2 < y1) { swap(y1, y2); swap(x1, x2); swap(u1, u2); swap(v1, v2); swap(w1, w2); }
	if (y3 < y1) { swap(y1, y3); swap(x1, x3); swap(u1, u3); swap(v1, v3); swap(w1, w3); }
	if (y3 < y2) { swap(y2, y3); swap(x2, x3); swap(u2, u3); swap(v2, v3); swap(w2, w3); }

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		  du1_step = 0, dv1_step = 0,
		  du2_step = 0, dv2_step = 0,
		  dw1_step = 0, dw2_step = 0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1)
	{
		for (int i = y1; i <= y2; i++)
		{
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u1 + (float)(i - y1) * du1_step;
			float tex_sv = v1 + (float)(i - y1) * dv1_step;
			float tex_sw = w1 + (float)(i - y1) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx) { swap(ax, bx); swap(tex_su, tex_eu); swap(tex_sv, tex_ev); swap(tex_sw, tex_ew); }

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > pDepthBuffer[i * render_state.width + j])
				{
					draw_pixel(j, i, tex->GetColorHexCodeUV(tex_u / tex_w, tex_v / tex_w));
					pDepthBuffer[i * render_state.width + j] = tex_w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1)
	{
		for (int i = y2; i <= y3; i++)
		{
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u2 + (float)(i - y2) * du1_step;
			float tex_sv = v2 + (float)(i - y2) * dv1_step;
			float tex_sw = w2 + (float)(i - y2) * dw1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx) { swap(ax, bx); swap(tex_su, tex_eu); swap(tex_sv, tex_ev); swap(tex_sw, tex_ew); }

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > pDepthBuffer[i * render_state.width + j])
				{
					draw_pixel(j, i, tex->GetColorHexCodeUV(tex_u / tex_w, tex_v / tex_w));
					pDepthBuffer[i * render_state.width + j] = tex_w;
				}
				t += tstep;
			}
		}
	}
}

internal void
render_test() {
	for (int x = 0; x < 1000; x += 20) {
		for (int y = 0; y < 1000; y += 20) {
			fill_rect(0 + x, 0 + y, 10, 10, COLOR_LIME);
			fill_rect(0 + x, 10 + y, 10, 10, COLOR_CYAN);
			fill_rect(10 + x, 0 + y, 10, 10, COLOR_MAGENTA);
			fill_rect(10 + x, 10 + y, 10, 10, COLOR_YELLOW);

			draw_rect(0 + x, 0 + y, 10, 10, COLOR_BLACK);
			draw_rect(0 + x, 10 + y, 10, 10, COLOR_BLACK);
			draw_rect(10 + x, 0 + y, 10, 10, COLOR_BLACK);
			draw_rect(10 + x, 10 + y, 10, 10, COLOR_BLACK);
		}
	}

	vector<int2> points;

	points.push_back({ 280, 150 });
	points.push_back({ 310, 100 });
	points.push_back({ 170, 150 });
	points.push_back({ 300, 50 });
	points.push_back({ 260, 10 });
	points.push_back({ 220, 100 });

	fill_polygon(points, COLOR_RED);
	draw_polygon(points, COLOR_BLACK);

	points.clear();

	points.push_back({ 300, 100 });
	points.push_back({ 350, 50 });
	points.push_back({ 400, 100 });
	points.push_back({ 350, 150 });

	fill_polygon(points, COLOR_GREEN);
	draw_polygon(points, COLOR_BLACK);

	points.clear();

	points.push_back({ 20, 65 });
	points.push_back({ 60, 10 });
	points.push_back({ 120, 5 });
	points.push_back({ 190, 150 });
	points.push_back({ 110, 70 });
	points.push_back({ 90, 150 });
	points.push_back({ 60, 55 });

	fill_polygon(points, COLOR_BLUE);
	draw_polygon(points, COLOR_BLACK);

	fill_ellipse(10, 10, 20, COLOR_MAROON);
	fill_ellipse(100, 200, 50, COLOR_OLIVE);
	fill_ellipse(200, 50, 30, COLOR_NAVY);
	fill_ellipse(250, 200, 40, COLOR_PURPLE);
}
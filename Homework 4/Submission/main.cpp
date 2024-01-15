#include <iostream>
#include <fstream>

#include "Shader.h"
#include "bezierCurve.h"
#include "mesh.h"
#include "viewer.h"

int main(int argc, char* argv[]) 
{
	const char* path = argv[1];
	std::string path_str = path;

	//////////////////////////////
	// Bezier curve viewer code //
	//////////////////////////////

	if (path_str.substr(path_str.length()-4, 4) == ".bzc")
	{
		// Each file contains a single Bezier curve's control points
		FILE* file = fopen(path, "r");

		int numControlPoints;
		fscanf(file, "%d", &numControlPoints);

		BezierCurve curve(numControlPoints);
		curve.loadControlPoints(file);
		fclose(file);

		// Create viewer
		Viewer viewer = Viewer();
		viewer.set_renderer(&curve);
		viewer.init();
		viewer.start();

		exit(EXIT_SUCCESS);
		return 0;
	}

	if (path_str.substr(path_str.length()-4, 4) == ".bez")
	{
		Mesh mesh;
		mesh.loadBezierMeshFile(path_str);

		// create viewer
		Viewer viewer = Viewer();
		viewer.set_renderer(&mesh);
		viewer.init();
		viewer.start();
		exit(EXIT_SUCCESS);
		return 0;
	}

	return 0;
}
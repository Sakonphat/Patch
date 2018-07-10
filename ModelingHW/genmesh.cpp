#include "genmesh.h"


// ---------------------------------------------------------------
// Helper function to compute normal vector
// We need normal vector to shade the model (you will learn shading in API class)

glm::vec3 computeNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
	glm::vec3 v21 = v2 - v1;
	glm::vec3 v31 = v3 - v1;
	return normalize(cross(v21, v31));
}



// ---------------------------------------------------------------
// Modify this function to Implement Sweep or Revolve
// We will use Triangle list data structure to represent our polygon mesh
// (The example code generates a sphere)
void FindSecondaryControlPoints(glm::vec3** control_points, int num_point, int t,float segment) {
	int num = num_point;
	float nT = t / segment;

	for (int i = 1; i < num_point; i++) {
		num--;
		for (int j = 0; j < num; j++) {

			control_points[i][j].x = ((1.0 - nT)*(control_points[i - 1][j].x)) + (nT*(control_points[i - 1][j + 1].x));
			control_points[i][j].y = ((1.0 - nT)*(control_points[i - 1][j].y)) + (nT*(control_points[i - 1][j + 1].y));
			control_points[i][j].z = ((1.0 - nT)*(control_points[i - 1][j].z)) + (nT*(control_points[i - 1][j + 1].z));

		}
	}
}

void GenerateBezierPatch(std::vector<glm::vec3> & out_vertices, std::vector<glm::vec3> & out_normals, float segment)
{
	// Clear the array first
	out_vertices.clear();
	out_normals.clear();

	glm::vec3 point[4][5], **controlPoint,**answerCP;
	glm::vec3 newControlPoint[4];
	int num = 0;

	answerCP = new glm::vec3*[segment];
	for (int i = 0; i < segment; i++) {
		answerCP[i] = new glm::vec3[segment];
	}
	
	controlPoint = new glm::vec3*[4];
	for (int i = 0; i < 4; i++) {
		controlPoint[i] = new glm::vec3[5];
	}

	point[0][0] = glm::vec3(0, 1, 0);
	point[0][1] = glm::vec3(2, 1, 0);
	point[0][2] = glm::vec3(4, 1, 0);
	point[0][3] = glm::vec3(6, 1, 0);
	point[0][4] = glm::vec3(8, 1, 0);

	point[1][0] = glm::vec3(0, -4, 2);
	point[1][1] = glm::vec3(2, -4, 2);
	point[1][2] = glm::vec3(4, -4, 2);
	point[1][3] = glm::vec3(6, -4, 2);
	point[1][4] = glm::vec3(8, -4, 2);

	point[2][0] = glm::vec3(0, 5, 4);
	point[2][1] = glm::vec3(2, 5, 4);
	point[2][2] = glm::vec3(4, 5, 4);
	point[2][3] = glm::vec3(6, 5, 4);
	point[2][4] = glm::vec3(8, 5, 4);

	point[3][0] = glm::vec3(0, 1, 6);
	point[3][1] = glm::vec3(2, 1, 6);
	point[3][2] = glm::vec3(4, 1, 6);
	point[3][3] = glm::vec3(6, 1, 6);
	point[3][4] = glm::vec3(8, 1, 6);

	for (int i = 0; i < segment; i++) {
		for (int j = 0; j < segment; j++) {
			//find j
			controlPoint[0][0] = point[0][0];
			controlPoint[0][1] = point[0][1];
			controlPoint[0][2] = point[0][2];
			controlPoint[0][3] = point[0][3];
			controlPoint[0][3] = point[0][4];
			FindSecondaryControlPoints(controlPoint,4,j, segment);
			newControlPoint[0] = controlPoint[3][0];

			controlPoint[0][0] = point[1][0];
			controlPoint[0][1] = point[1][1];
			controlPoint[0][2] = point[1][2];
			controlPoint[0][3] = point[1][3];
			controlPoint[0][3] = point[1][4];
			FindSecondaryControlPoints(controlPoint, 4, j, segment);
			newControlPoint[1] = controlPoint[3][0];

			controlPoint[0][0] = point[2][0];
			controlPoint[0][1] = point[2][1];
			controlPoint[0][2] = point[2][2];
			controlPoint[0][3] = point[2][3];
			controlPoint[0][3] = point[2][4];
			FindSecondaryControlPoints(controlPoint, 4, j, segment);
			newControlPoint[2] = controlPoint[3][0];

			controlPoint[0][0] = point[3][0];
			controlPoint[0][1] = point[3][1];
			controlPoint[0][2] = point[3][2];
			controlPoint[0][3] = point[3][3];
			controlPoint[0][3] = point[3][4];
			FindSecondaryControlPoints(controlPoint, 4, j, segment);
			newControlPoint[3] = controlPoint[3][0];

			//find i
			controlPoint[0][0] = newControlPoint[0];
			controlPoint[0][1] = newControlPoint[1];
			controlPoint[0][2] = newControlPoint[2];
			controlPoint[0][3] = newControlPoint[3];
			controlPoint[0][4] = newControlPoint[4];
			FindSecondaryControlPoints(controlPoint, 4, i, segment);
			answerCP[i][j] = controlPoint[3][0];
		}
	}

	// build 2 traingle from 4 points on each loop
	// dont forget about counter clockwise ordering

	//square
	for (int i = 0; i < segment-1; i++) {
		for (int j = 0; j < segment-1; j++) {
			out_vertices.push_back(answerCP[i+1][j]);
			out_vertices.push_back(answerCP[i+1][j+1]);
			out_vertices.push_back(answerCP[i][j+1]);
			std::cout << i << " " << j << " : "<< answerCP[i + 1][j].x << " " << answerCP[i + 1][j].y  << " " << answerCP[i + 1][j].z << std::endl;
			out_vertices.push_back(answerCP[i][j+1]);
			out_vertices.push_back(answerCP[i][j]);
			out_vertices.push_back(answerCP[i+1][j]);
		}
	}

	// compute the normal vector for each triangle
	// but in our OpenGL program, the normal vector is a property of a vertex
	// so we have to push the same normal vector to the array, one for each vertex 
	for (int i = 0; i < segment-1; i++) {
		for (int j = 0; j < segment-1; j++) {
			out_normals.push_back(computeNormal(answerCP[i + 1][j], answerCP[i + 1][j + 1], answerCP[i][j + 1]));
			out_normals.push_back(computeNormal(answerCP[i + 1][j], answerCP[i + 1][j + 1], answerCP[i][j + 1]));
			out_normals.push_back(computeNormal(answerCP[i + 1][j], answerCP[i + 1][j + 1], answerCP[i][j + 1]));

			out_normals.push_back(computeNormal(answerCP[i][j + 1], answerCP[i][j], answerCP[i + 1][j]));
			out_normals.push_back(computeNormal(answerCP[i][j + 1], answerCP[i][j], answerCP[i + 1][j]));
			out_normals.push_back(computeNormal(answerCP[i][j + 1], answerCP[i][j], answerCP[i + 1][j]));
		}
	}


	
}

#define _USE_MATH_DEFINES

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <limits>
#include <functional>
#include <algorithm>

#include "Geometry.h"
#include "GLDebug.h"
#include "GLHandles.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Panel.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "cmath"

#define POINT_THRESHOLD 0.075f
#define CAMERA_DEFAULT_DISTANCE 1.f

#define VERTICAL_SENSITIVITY 0.1f
#define HORIZONTAL_SENSITIVITY 0.15f

struct Camera {
	float yaw = M_PI_2;
	float pitch = 0.0f;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, CAMERA_DEFAULT_DISTANCE);
	glm::vec3 last_position = glm::vec3(0.0f, 0.0f, CAMERA_DEFAULT_DISTANCE);
};

struct Parameters {
	glm::vec3 cursor_pos = { 0.f,0.f,0.f };
	glm::vec2 window_size = { 800.f, 800.f };

	glm::vec3 cp_point_colour = { 1.f, 0.f, 0.f }; // Red color for control points
	glm::vec3 cp_line_colour = { 0.f, 1.f, 0.f }; // Blue color for control point lines

	std::vector<glm::vec3> cp_positions_vector = {
		{0.f, 0.f, 0.f},
	};
	std::vector<glm::vec3> cp_colours_vector = std::vector<glm::vec3>(cp_positions_vector.size(), cp_point_colour);
	std::vector<glm::vec3> cp_lines_colours_vector = std::vector<glm::vec3>(cp_positions_vector.size(), cp_line_colour);

	int select = -1; // Selected Control Point
	int scene = 0; // 0: default, 1: Bezier, 2: B-Spline, 3: Surface of Revolution

	bool view3D = false;

	bool render_cp = true;
	bool render_cp_lines = true;

	// Camera parameters
	Camera cam;

	bool start_drag = false;
	bool dragging_camera = false;
	glm::vec3 drag_start = { 0.f, 0.f, 0.f };

	glm::mat4 projection = glm::perspective(
		glm::radians(45.f),
		window_size.x / window_size.y,
		0.1f, 100.0f);

	glm::mat4 view_transform = glm::mat4(1.0);

};

static Parameters params;

class CurveEditorCallBack : public CallbackInterface {
public:
	CurveEditorCallBack() {}

	virtual void keyCallback(int key, int scancode, int action, int mods) override {
		Log::info("KeyCallback: key={}, action={}", key, action);
		if (action == GLFW_PRESS) {

			if (key == GLFW_KEY_1) {
				params.scene = 1;
			} else if (key == GLFW_KEY_2) {
				params.scene = 2;
			} else if (key == GLFW_KEY_3) {
				params.scene = 3;
			}
		}
	}

	virtual void mouseButtonCallback(int button, int action, int mods) override {
		// Log::info("MouseButtonCallback: button={}, action={}", button, action);
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {	
			params.select = selectControlPoint(params.cursor_pos, POINT_THRESHOLD);
		
			if (params.select == -1) {
				params.cp_positions_vector.push_back(params.cursor_pos);
				params.cp_colours_vector.push_back(params.cp_point_colour);
				params.cp_lines_colours_vector.push_back(params.cp_line_colour);
			}

		} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			if (params.select != -1) {
				params.cp_colours_vector[params.select] = { 1.f, 0.f, 0.f };
			}
			params.select = -1;

		} else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			int selection = selectControlPoint(params.cursor_pos, POINT_THRESHOLD);
			if (selection != -1) {
				params.cp_positions_vector.erase(params.cp_positions_vector.begin() + selection);
				params.cp_colours_vector.erase(params.cp_colours_vector.begin() + selection);
			}
			params.select = -1;
		}
	}

	virtual void cursorPosCallback(double xpos, double ypos) override {
		params.cursor_pos = calculateMousePos(xpos, ypos);

		if (params.select != -1) {
			params.cp_colours_vector[params.select] = { 0.f, 1.f, 0.f };
			params.cp_positions_vector[params.select] = params.cursor_pos;
		}
		// Log::info("CursorPosCallback: xpos={}, ypos={}", params.cursor_pos.x, params.cursor_pos.y);
	}

	virtual void scrollCallback(double xoffset, double yoffset) override {
	}

	virtual void windowSizeCallback(int width, int height) override {
		Log::info("WindowSizeCallback: width={}, height={}", width, height);
		CallbackInterface::windowSizeCallback(width, height); // Important, calls glViewport(0, 0, width, height);
		params.window_size = { width, height };
	}

private:
	glm::vec3 calculateMousePos(double xpos, double ypos) {
		glm::vec4 cursor = {xpos, ypos, 0.f, 1.f};

		glm::mat4 pixel_centering_T = glm::translate(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.f));
		glm::mat4 zero2one_T = glm::scale(glm::mat4(1.f), glm::vec3(1.f / params.window_size.x, 1.f / params.window_size.y, 0.f));
		cursor = zero2one_T * pixel_centering_T * cursor;

		cursor.y = 1.f - cursor.y;

		glm::mat4 normalize_S = glm::scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 1.f));
		glm::mat4 normalize_T = glm::translate(glm::mat4(1.f), glm::vec3(-1.f, -1.f, 0.f));
		cursor = normalize_T * normalize_S * cursor;

		return glm::vec3(cursor.x, cursor.y, 0.f);
	}

	int selectControlPoint(glm::vec3 cursor_pos, float threshold) {
		int selection = -1;
		for (size_t i = 0; i < params.cp_positions_vector.size(); i++) {
			if (glm::length(glm::vec2(params.cp_positions_vector[i]) - glm::vec2(params.cursor_pos)) < threshold) {
				Log::info("Selected control point {}", i);
				selection = i;
				break;
			}
		}

		params.select = selection;
		return selection;
	}
};

class TurnTable3DViewerCallBack : public CallbackInterface {

public:
	TurnTable3DViewerCallBack() {}

	virtual void keyCallback(int key, int scancode, int action, int mods) override {
		Log::info("KeyCallback: key={}, action={}", key, action);
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_1) {
				params.scene = 1;
			} else if (key == GLFW_KEY_2) {
				params.scene = 2;
			} else if (key == GLFW_KEY_3) {
				params.scene = 3;
			}

			else if (key == GLFW_KEY_R) {
				params.cam.yaw = M_PI_2;
				params.cam.pitch = 0.0f;
				params.cam.position = glm::vec3(0.0f, 0.0f, CAMERA_DEFAULT_DISTANCE);
			}
		}
	}

	virtual void mouseButtonCallback(int button, int action, int mods) {
		if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			params.start_drag = true;
			params.dragging_camera = true;
	
		} else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			params.dragging_camera = false;
		}

	}
	virtual void cursorPosCallback(double xpos, double ypos) {
		// Calculate the new mouse position in your chosen coordinate system
		params.cursor_pos = calculateMousePos(xpos, ypos);

		// Start dragging: initialize necessary parameters (first time mouse press)
		if (params.start_drag) {
			params.drag_start = params.cursor_pos;
			params.cam.last_position = params.cam.position;
			params.start_drag = false;
		}

		if (params.dragging_camera) {
			// Calculate displacement
			glm::vec3 displacement = (params.cursor_pos - params.drag_start) * -0.1f;

			// Calculate new yaw and pitch based on displacement
			float yaw_offset = -displacement.x * HORIZONTAL_SENSITIVITY;
			float pitch_offset = clampDisplacement(displacement.y) * VERTICAL_SENSITIVITY;

			params.cam.yaw += yaw_offset;
			params.cam.pitch += pitch_offset;

			// Clamp pitch to avoid gimbal lock
			if (params.cam.pitch > ( M_PI_2 - 0.05f)) params.cam.pitch = (M_PI_2 - 0.05f);
			if (params.cam.pitch < (-M_PI_2 + 0.05f)) params.cam.pitch = (-M_PI_2 + 0.05f);

			// Update camera direction (front vector) based on new yaw and pitch
			glm::vec3 front;
			front.x = cos(params.cam.yaw) * cos(params.cam.pitch);
			front.y = sin(params.cam.pitch);
			front.z = sin(params.cam.yaw) * cos(params.cam.pitch);

			// Update camera position based on new front vector
			front = glm::normalize(front);
			float radius = glm::length(params.cam.last_position); // Distance from origin

			params.cam.position = front * radius;
		}
	}
	virtual void scrollCallback(double xoffset, double yoffset) {
		// get vector to origin from camera
		glm::vec3 to_origin = glm::normalize(params.cam.position);

		// move camera along vector
		params.cam.position -= to_origin * float(yoffset) * 0.1f;
	}
	virtual void windowSizeCallback(int width, int height) {

		// The CallbackInterface::windowSizeCallback will call glViewport for us
		CallbackInterface::windowSizeCallback(width, height);
	}
private:
	glm::vec3 calculateMousePos(double xpos, double ypos) {
		glm::vec4 cursor = {xpos, ypos, 0.f, 1.f};

		glm::mat4 pixel_centering_T = glm::translate(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.f));
		glm::mat4 zero2one_T = glm::scale(glm::mat4(1.f), glm::vec3(1.f / params.window_size.x, 1.f / params.window_size.y, 0.f));
		cursor = zero2one_T * pixel_centering_T * cursor;

		cursor.y = 1.f - cursor.y;

		glm::mat4 normalize_S = glm::scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 1.f));
		glm::mat4 normalize_T = glm::translate(glm::mat4(1.f), glm::vec3(-1.f, -1.f, 0.f));
		cursor = normalize_T * normalize_S * cursor;

		return glm::vec3(cursor.x, cursor.y, 0.f);
	}

	float clampDisplacement(float displacement) {
		if (displacement > 0.15f) {
			return 0.15f;
		}
		else if (displacement < -0.15f) {
			return -0.15f;
		}
		return displacement;
	}
};

class CurveEditorPanelRenderer : public PanelRendererInterface {
public:
	CurveEditorPanelRenderer()
		: render_cp(true), render_cp_lines(true), view_3D(false)
	{
		// Initialize options for the combo box
		options[0] = "Option 1";
		options[1] = "Option 2";
		options[2] = "Option 3";

		// Initialize color (black by default)
		colorValue[0] = 0.f; // R
		colorValue[1] = 0.f; // G
		colorValue[2] = 0.f; // B
	}

	virtual void render() override {
		// Color selector
		ImGui::ColorEdit3("Select Background Color", colorValue); // RGB color selector
		ImGui::Text("Selected Color: R: %.3f, G: %.3f, B: %.3f", colorValue[0], colorValue[1], colorValue[2]);

		// Render Control Points Checkbox
		ImGui::Checkbox("Render Control Points", &render_cp);
		ImGui::SameLine();
		// Render Control Point Lines Checkbox
		ImGui::Checkbox("Render Control Point Lines", &render_cp_lines);
		ImGuiAddSpace();
		// 3D View Checkbox
		ImGui::Checkbox("3D View", &view_3D);

		// Scene selection
		ImGuiAddSpace();
		ImGui::Text("Select Scene:");

		ImGui::RadioButton("None", &params.scene, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Bezier", &params.scene, 1);
		ImGui::SameLine();
		ImGui::RadioButton("B-Spline", &params.scene, 2);
		
		ImGui::RadioButton("Surface of Revolution", &params.scene, 3);
	}

	void ImGuiAddSpace() {
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	glm::vec3 getColor() const {
		return glm::vec3(colorValue[0], colorValue[1], colorValue[2]);
	}

	bool getRenderCP() const {
		return render_cp;
	}

	bool getRenderCPLines() const {
		return render_cp_lines;
	}

	bool getView3D() const {
		return view_3D;
	}

private:
	float colorValue[3];  // Array for RGB color values
	bool render_cp;   // Value for checkbox
	bool render_cp_lines;   // Value for checkbox
	bool view_3D;   // Value for checkbox

	const char* options[3]; // Options for the combo box
};

glm::vec3 deCasteljau(const std::vector<glm::vec3>& controlPoints, float t) {
	if (controlPoints.size() < 1) {
		return glm::vec3(0,0,0);
	}
	std::vector<glm::vec3> temp = controlPoints;
	for (int j = 1; j < int(temp.size()); ++j) {
		for (int i = 0; i < int(temp.size()) - j; ++i) {
			temp[i] = (1.0f - t) * temp[i] + t * temp[i + 1];
		}
	}
	return temp[0];
}

std::vector<glm::vec3> quadraticBSpline(const std::vector<glm::vec3>& controlPoints, int iterations = 10) {
	if (controlPoints.size() < 3) {
		return std::vector<glm::vec3>{glm::vec3(0, 0, 0)};

	} else if (iterations == 0) {
		return controlPoints;
	}

	std::vector<glm::vec3> temp = controlPoints;

    std::vector<glm::vec3> curve_points;
    for (int i = 0; i < int(temp.size() - 2); i++) {
        curve_points.push_back((0.75f) * temp[i] + (0.25f) * temp[i + 1]);
        curve_points.push_back((0.25f) * temp[i] + (0.75f) * temp[i + 1]);
    }
    curve_points.push_back((0.75f) * temp[temp.size() - 2] + (0.25f) * temp[temp.size() - 1]);
    curve_points.push_back((0.25f) * temp[temp.size() - 2] + (0.75f) * temp[temp.size() - 1]);

	if (params.cp_positions_vector.size() > 2) {
		std::reverse(curve_points.begin(), curve_points.end());
		curve_points.push_back(params.cp_positions_vector.front());
		std::reverse(curve_points.begin(), curve_points.end());
		curve_points.push_back(params.cp_positions_vector.back());
	}

    return quadraticBSpline(curve_points, iterations - 1);
}

std::vector<glm::vec3> createSoR(const std::vector<glm::vec3>& curve_points, int n_slices = 36) {
	std::vector<glm::vec3> surface_points;
	float angle = 2 * M_PI / n_slices;

	for(int i = 0; i < curve_points.size() - 1; i++) {
		glm::vec3 p1 = curve_points[i];
		glm::vec3 p2 = curve_points[i + 1];

		for(int j = 0; j < n_slices; j++) {
			float theta = j * angle;
			float next_theta = (j + 1) * angle;

			glm::vec3 v1(p1.x * cos(theta), p1.y, p1.x * sin(theta));
			glm::vec3 v2(p2.x * cos(theta), p2.y, p2.x * sin(theta));
			glm::vec3 v3(p2.x * cos(next_theta), p2.y, p2.x * sin(next_theta));
			glm::vec3 v4(p1.x * cos(next_theta), p1.y, p1.x * sin(next_theta));

			surface_points.push_back(v1);
			surface_points.push_back(v2);
			surface_points.push_back(v3);

			surface_points.push_back(v1);
			surface_points.push_back(v3);
			surface_points.push_back(v4);
		}
	}
	
	return surface_points;
}

int main() {
	Log::debug("Starting main");

	glfwInit();
	Window window(800, 800, "CPSC 453: Assignment 3");
	Panel panel(window.getGLFWwindow());

	auto curve_editor_callback = std::make_shared<CurveEditorCallBack>();
	auto curve_editor_panel_renderer = std::make_shared<CurveEditorPanelRenderer>();

	window.setCallbacks(curve_editor_callback);
	panel.setPanelRenderer(curve_editor_panel_renderer);

	params.window_size = { window.getWidth(), window.getHeight()};

	ShaderProgram shader_program_default("shaders/test.vert", "shaders/test.frag");

	// Control points
	CPU_Geometry cp_cpu;
	cp_cpu.verts = params.cp_positions_vector;
	cp_cpu.cols = params.cp_colours_vector;

	GPU_Geometry cp_gpu;
	cp_gpu.setVerts(cp_cpu.verts);
	cp_gpu.setCols(cp_cpu.cols);

	// Contol point lines
	CPU_Geometry cp_lines_cpu;
	cp_lines_cpu.verts = params.cp_positions_vector;
	cp_lines_cpu.cols = params.cp_lines_colours_vector;

	GPU_Geometry cp_lines_gpu;
	cp_lines_gpu.setVerts(cp_lines_cpu.verts);
	cp_lines_gpu.setCols(cp_lines_cpu.cols);

	// Curve Points
	CPU_Geometry curve_cpu;
	GPU_Geometry curve_gpu;

	std::vector<glm::vec3> curve_points;
	int segments = 100;

	// Surface of Revolution
	std::vector<glm::vec3> surface_points;

	CPU_Geometry surface_cpu;
	GPU_Geometry surface_gpu;

	// Tensor Product Surface
	std::vector<glm::vec3> tensor_cp;

	std::vector<glm::vec3> tensor_cp2;

	CPU_Geometry tensor_cpu;
	GPU_Geometry tensor_gpu;

	while (!window.shouldClose()) {
		params.window_size = { window.getWidth(), window.getHeight() };

		params.render_cp = curve_editor_panel_renderer->getRenderCP();
		params.render_cp_lines = curve_editor_panel_renderer->getRenderCPLines();
		params.view3D = curve_editor_panel_renderer->getView3D();

		if (params.view3D) {
			window.setCallbacks(std::make_shared<TurnTable3DViewerCallBack>());
		}
		else {
			window.setCallbacks(curve_editor_callback);
		}

		glfwPollEvents();
		glm::vec3 background_colour = curve_editor_panel_renderer->getColor();

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(background_colour.r, background_colour.g, background_colour.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader_program_default.use();

		// View Camera
		glm::mat4 view = glm::lookAt(
			params.cam.position,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		if (!params.view3D) {
			params.view_transform = glm::mat4(1.0);
		} else {
			params.view_transform = params.projection * view;
		}

		glUniformMatrix4fv(
			glGetUniformLocation(shader_program_default.getProgram(), "view_transform"),
			1, GL_FALSE, glm::value_ptr(params.view_transform)
		);

		// Calculate points on the curve
		curve_cpu.verts = curve_points;
		switch (params.scene) {
		case 1: // Bezier

			curve_cpu.cols = std::vector<glm::vec3>(curve_cpu.verts.size(), glm::vec3(0, 0, 1)); // Blue color for Bézier curve

			curve_points.clear();
			for (int i = 0; i <= segments; ++i) {
				float t = i / (float)segments;
				curve_points.push_back(deCasteljau(params.cp_positions_vector, t));
			}

			break;

		case 2: // B-Spline
			curve_cpu.cols = std::vector<glm::vec3>(curve_cpu.verts.size(), glm::vec3(0.25, 0, 1)); // Purple color for B-Spline curve

			curve_points.clear();
			curve_points = quadraticBSpline(params.cp_positions_vector);

			break;

		case 3: // Surface of Revolution
			surface_points.clear();
			surface_points = createSoR(quadraticBSpline(params.cp_positions_vector, 3));

			surface_cpu.verts = surface_points;
			surface_cpu.cols = std::vector<glm::vec3>(surface_cpu.verts.size(), glm::vec3(1, 1, 1)); // White color for Surface of Revolution

			break;
			
		default:
			break;
		}

		// Render control points
		if (params.render_cp) {
			cp_cpu.verts = params.cp_positions_vector;
			cp_cpu.cols = params.cp_colours_vector;

			cp_gpu.setVerts(cp_cpu.verts);
			cp_gpu.setCols(cp_cpu.cols);

			cp_gpu.bind();
			glPointSize(15.f);
			glDrawArrays(GL_POINTS, 0, cp_cpu.verts.size());
		}

		// Render curve
		if (params.scene == 1 || params.scene == 2) {
			curve_gpu.setVerts(curve_points);
			curve_gpu.setCols(curve_cpu.cols);

			curve_gpu.bind();
			glDrawArrays(GL_LINE_STRIP, 0, curve_cpu.verts.size());
		}

		if (params.scene == 3){ // Render Surface of Revolution
			surface_gpu.setVerts(surface_cpu.verts);
			surface_gpu.setCols(surface_cpu.cols);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			surface_gpu.bind();
			glDrawArrays(GL_TRIANGLES, 0, surface_cpu.verts.size());
		}
		
		// Render control point lines
		if (params.render_cp_lines) {
			cp_lines_cpu.verts = params.cp_positions_vector;
			cp_lines_cpu.cols = params.cp_lines_colours_vector;

			cp_lines_gpu.setVerts(cp_lines_cpu.verts);
			cp_lines_gpu.setCols(cp_lines_cpu.cols);

			cp_lines_gpu.bind();
			glDrawArrays(GL_LINE_STRIP, 0, cp_lines_cpu.verts.size());
		}

		//
		glDisable(GL_FRAMEBUFFER_SRGB);
		panel.render();
		window.swapBuffers();
	}

	glfwTerminate();
	return 0;
}

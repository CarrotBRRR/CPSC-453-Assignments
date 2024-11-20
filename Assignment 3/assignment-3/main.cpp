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
	int scene = 0; // 0: default, 1: Bezier, 2: B-Spline

	bool view3D = false;

	bool render_cp = true;
	bool render_cp_lines = true;

	// Camera parameters
	bool start_drag = false;
	bool dragging_camera = false;

	glm::vec3 drag_start = { 0.f, 0.f, 0.f };

	glm::vec3 last_camera_position = glm::vec3(0.0f, 0.0f, 2.415f);
	glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 2.415f);

	glm::mat4 projection = glm::perspective(
		glm::radians(45.0f),
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
			}

			else if (key == GLFW_KEY_R) {
				params.camera_position = glm::vec3(0.0f, 0.0f, 2.415f);
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
		params.cursor_pos = calculateMousePos(xpos, ypos);

		if(params.start_drag) {
			params.drag_start = params.cursor_pos;
			params.last_camera_position = params.camera_position;
			params.start_drag = false;

			Log::info("CursorPosCallback: Drag_X={}, Drag_Y={}", params.drag_start.x, params.drag_start.y);
		}
		if (params.dragging_camera) {
			glm::vec3 displacement = (params.cursor_pos - params.drag_start) * -0.1f;

			Log::info("CursorPosCallback: Displacement_X={}, Displacement_Y={}", displacement.x, displacement.y);

			// get distance to origin from camera
			glm::vec3 to_origin = glm::normalize(params.last_camera_position);
			float radius = glm::length(params.last_camera_position);

			// Rotation Angles (Yaw and Pitch, in Radians) with relation to displacement
			float yaw = displacement.x * 6 * M_PI;
			float pitch = displacement.y * 6 * M_PI;

			pitch = glm::clamp((double)pitch, -M_PI_2 + 0.01f, M_PI_2 - 0.01f);

			// Rotation Matrix
			glm::mat4 yaw_rotation = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis
			glm::mat4 pitch_rotation = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1.0f, 0.0f, 0.0f)); // X-axis

			// Combine rotations
			glm::mat4 rotation =  yaw_rotation * pitch_rotation;

			glm::vec4 new_position = rotation * glm::vec4(params.last_camera_position, 1.0f);

			params.camera_position = glm::normalize(glm::vec3(new_position)) * radius;
		}
	}
	virtual void scrollCallback(double xoffset, double yoffset) {
		// get vector to origin from camera
		glm::vec3 to_origin = glm::normalize(params.camera_position);

		// move camera along vector
		params.camera_position -= to_origin * float(yoffset) * 0.1f;
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
		ImGui::Text("Feature Enabled: %s", render_cp ? "Yes" : "No");

		// Render Control Point Lines Checkbox
		ImGui::Checkbox("Render Control Point Lines", &render_cp_lines);
		ImGui::Text("Feature Enabled: %s", render_cp_lines ? "Yes" : "No");

		// 3D View Checkbox
		ImGui::Checkbox("3D View", &view_3D);
		ImGui::Text("Feature Enabled: %s", view_3D ? "Yes" : "No");

		//// Display the input text
		//ImGui::Text("You entered: %s", inputText);

		//// Text input
		//ImGui::InputText("Input Text", inputText, IM_ARRAYSIZE(inputText));

		//// Button
		//if (ImGui::Button("Click Me")) {
		//	buttonClickCount++;
		//}
		//ImGui::Text("Button clicked %d times", buttonClickCount);

		//// Scrollable block
		//ImGui::TextWrapped("Scrollable Block:");
		//ImGui::BeginChild("ScrollableChild", ImVec2(0, 100), true); // Create a scrollable child
		//for (int i = 0; i < 20; i++) {
		//	ImGui::Text("Item %d", i);
		//}
		//ImGui::EndChild();

		//// Float slider
		//ImGui::SliderFloat("Float Slider", &sliderValue, 0.0f, 100.0f, "Slider Value: %.3f");

		//// Float drag
		//ImGui::DragFloat("Float Drag", &dragValue, 0.1f, 0.0f, 100.0f, "Drag Value: %.3f");

		//// Float input
		//ImGui::InputFloat("Float Input", &inputValue, 0.1f, 1.0f, "Input Value: %.3f");

		//// Combo box
		//ImGui::Combo("Select an Option", &comboSelection, options, IM_ARRAYSIZE(options));
		//ImGui::Text("Selected: %s", options[comboSelection]);

		//// Displaying current values
		//ImGui::Text("Slider Value: %.3f", sliderValue);
		//ImGui::Text("Drag Value: %.3f", dragValue);
		//ImGui::Text("Input Value: %.3f", inputValue);
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

	//char inputText[256];  // Buffer for input text
	//int buttonClickCount; // Count button clicks
	//float sliderValue;    // Value for float slider
	//float dragValue;      // Value for drag input
	//float inputValue;     // Value for float input
	//int comboSelection;   // Index of selected option in combo box
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

std::vector<glm::vec3> quadraticBSpline(const std::vector<glm::vec3>& controlPoints, int iterations = 15) {
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

    return quadraticBSpline(curve_points, iterations - 1);
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

	// Set up control points in GPU
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

	// Curve points
	std::vector<glm::vec3> curve_points;
	int segments = 100;

	// Set up curve in GPU
	CPU_Geometry curve_cpu;
	GPU_Geometry curve_gpu;

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
			params.camera_position,
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

			if (params.cp_positions_vector.size() > 2) {
				std::reverse(curve_points.begin(), curve_points.end());
				curve_points.push_back(params.cp_positions_vector.front());
				std::reverse(curve_points.begin(), curve_points.end());
				curve_points.push_back(params.cp_positions_vector.back());
			}
			
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
		curve_gpu.setVerts(curve_points);
		curve_gpu.setCols(curve_cpu.cols);

		curve_gpu.bind();
		glDrawArrays(GL_LINE_STRIP, 0, curve_cpu.verts.size());
		
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

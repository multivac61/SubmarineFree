//SubTag DS TM TW W8

#include "DS.hpp"

namespace {

	unsigned char colors[11 * 3] = {
		32, 30, 36,
		201, 185, 15,
		12, 142, 21, 
		201, 24, 71,
		9, 134, 173,
		255, 255, 255,
		255, 174, 201,
		185, 00, 183,
		128, 128, 128,
		255, 153, 65,
		255, 174, 201
	};

	typedef uint16_t status_t;

	void drawConnector(NVGcontext *vg, float x, float y, NVGcolor color) {
		nvgFillColor(vg, color);
		nvgBeginPath(vg);
		nvgCircle(vg, x, y, 4);
		nvgFill(vg);
	}

	inline void drawOutput(NVGcontext *vg, float leftPos) {
		nvgMoveTo(vg, leftPos, 30);
		nvgLineTo(vg, 81, 30);
	}

	inline void drawInput1of1(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 8);
		nvgLineTo(vg, 20, 8);
		nvgLineTo(vg, 20, 30);
		nvgLineTo(vg, rightPos, 30);
	}

	inline void drawInput1of2(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 8);
		nvgLineTo(vg, 23, 8);
		nvgLineTo(vg, 23, 26);
		nvgLineTo(vg, rightPos, 26);
	}
	
	inline void drawInput2of2(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 23);
		nvgLineTo(vg, 17, 23);
		nvgLineTo(vg, 17, 34);
		nvgLineTo(vg, rightPos, 34);
	}

	inline void drawInput1of3(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 8);
		nvgLineTo(vg, 23, 8);
		nvgLineTo(vg, 23, 24);
		nvgLineTo(vg, rightPos, 24);
	}

	inline void drawInput2of3(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 23);
		nvgLineTo(vg, 17, 23);
		nvgLineTo(vg, 17, 30);
		nvgLineTo(vg, rightPos, 30);
	}

	inline void drawInput3of3(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 37);
		nvgLineTo(vg, 17, 37);
		nvgLineTo(vg, 17, 36);
		nvgLineTo(vg, rightPos, 36);
	}

	inline void drawInput1of4(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 8);
		nvgLineTo(vg, 23, 8);
		nvgLineTo(vg, 23, 21);
		nvgLineTo(vg, rightPos, 21);
	}
	
	inline void drawInput2of4(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 23);
		nvgLineTo(vg, 17, 23);
		nvgLineTo(vg, 17, 27);
		nvgLineTo(vg, rightPos, 27);
	}
	
	inline void drawInput3of4(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 37);
		nvgLineTo(vg, 17, 37);
		nvgLineTo(vg, 17, 33);
		nvgLineTo(vg, rightPos, 33);
	}
	
	inline void drawInput4of4(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 51);
		nvgLineTo(vg, 23, 51);
		nvgLineTo(vg, 23, 39);
		nvgLineTo(vg, rightPos, 39);
	}
	
	inline void drawAnd(NVGcontext *vg) {
		nvgMoveTo(vg, 33, 18);
		nvgBezierTo(vg, 61, 18, 61, 42, 33, 42);
		nvgLineTo(vg, 33, 18);
	}

	inline void drawOr(NVGcontext *vg) {
		nvgMoveTo(vg, 33, 18);
		nvgQuadTo(vg, 45, 18, 55, 30);
		nvgQuadTo(vg, 45, 42, 33, 42);
		nvgQuadTo(vg, 36, 30, 33, 18);
	}

	inline void drawXor(NVGcontext *vg) {
		nvgMoveTo(vg, 33, 17);
		nvgQuadTo(vg, 36, 30, 33, 43);
		nvgMoveTo(vg, 37, 18);
		nvgQuadTo(vg, 49, 18, 59, 30);
		nvgQuadTo(vg, 49, 42, 37, 42);
		nvgQuadTo(vg, 40, 30, 37, 18);
	}

	inline void drawNot(NVGcontext *vg, float xPos) {
		nvgEllipse(vg, xPos, 30, 4, 4);
	}

	inline void drawBox(NVGcontext *vg) {
		nvgRect(vg, 30, 10, 30, 40);
	}

	struct Functor {
		std::string name;
		std::function<void (const Widget::DrawArgs &, Vec size)> draw;
		std::function<status_t (status_t a,
					status_t b,
					status_t c,
					status_t d,
					status_t &a0,
					status_t &b0,
					status_t &c0,
					status_t &d0)> process;
		

	};

#define LAMBDA_HEADER [] (status_t a, status_t b, status_t c, status_t d, status_t &a0, status_t &b0, status_t &c0, status_t &d0) -> status_t

	std::vector<Functor> functions {
#include "gates/ShortCircuit"
,
#include "gates/NOT"
,
#include "gates/2-AND"
,
#include "gates/3-AND"
,
#include "gates/4-AND"
,
#include "gates/OR"
,
#include "gates/XOR"
,
#include "gates/2-NAND"
,
#include "gates/3-NAND"
,
#include "gates/4-NAND"
,
#include "gates/NOR"
,
#include "gates/XNOR"
,
#include "gates/D-TYPE-FLIPFLOP"
	};

#undef LAMBDA_HEADER

	struct PLConnectorRenderer : TransparentWidget {
		std::function<void (const Widget::DrawArgs &)> drawLambda;
		void draw(const DrawArgs &args) override {
			drawLambda(args);
			Widget::draw(args);
		}
	};

	struct PLGateKnob : TooltipKnob {
		Module *module;
		int index;
		PLGateKnob() {
			box.size.x = 86;
			box.size.y = 60;
			snap = true;
			smooth = false;
		}
		void draw(const DrawArgs &args) override {
			if (module) {
				unsigned int val = (unsigned int)APP->engine->getParam(module, index);
				if (val >= functions.size()) {
					val = functions.size() - 1;
				}
				functions[val].draw(args, box.size);
				unsigned int i = box.pos.y / 80;
				i += 6;
				drawConnector(args.vg, box.size.x - 5, box.size.y / 2.0f, nvgRGB(colors[i * 3], colors[i * 3 + 1], colors[i * 3 + 2]));
			}
		}
	};

	struct PLConnectorKnob : Knob {
		Module *module;
		float fade = 0.1f;
		PLConnectorKnob() {
			box.size.x = 10;
			box.size.y = 10;
			snap = true;
			smooth = false;
		}
		void step() override {
			if (fade > 0.1f)
				fade -= 0.02f;
		}
		void onDragStart(const event::DragStart &e) override {
			Knob::onDragStart(e);
			fade = 0.9f;
		}
		void onDragMove(const event::DragMove &e) override {
			Knob::onDragMove(e);
			fade = 0.9f;
		}
		void onDragEnd(const event::DragEnd &e) override {
			Knob::onDragEnd(e);
			fade = 0.9f;
		}
	};

	template<unsigned int x, unsigned int y>
	struct PLBackground : OpaqueWidget {
		void draw(const DrawArgs &args) override {
			nvgFillColor(args.vg, nvgRGB(0,0,0));
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
			nvgFill(args.vg);
			for (unsigned int ix = 0; ix < x + 2; ix++) {
				drawConnector(args.vg, box.size.x / (x * 2 + 4.0f) * (ix * 2 + 1), 5, nvgRGB(colors[ix * 3], colors[ix * 3 + 1], colors[ix * 3 + 2]));
			}
			nvgStrokeWidth(args.vg, 2);
			for (unsigned int ix = 0; ix < x; ix++) {
				nvgStrokeColor(args.vg, nvgRGB(colors[ix * 3 + 3], colors[ix * 3 + 4], colors[ix * 3 + 5]));
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, box.size.x / (x * 2 + 4.0f) * (ix * 2 + 3), 5);
				nvgLineTo(args.vg, 15 + ix * 30 - box.pos.x, 30 - box.pos.y);
				nvgStroke(args.vg);
			}
			Widget::draw(args);
		}
	};
}

template <unsigned int x, unsigned int y>
struct DO1 : DS_Module {
	enum ParamIds {
		PARAM_GATE_1,
		PARAM_CONNECTOR_1 = y,
		PARAM_CONNECTOR_2,
		PARAM_CONNECTOR_3,
		PARAM_CONNECTOR_4,
		PARAM_CONNECTOR_OUT_1 = y + 4 * y,
		NUM_PARAMS = y + x + 4 * y
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS = x
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = x
	};
	enum LightIds {
		NUM_LIGHTS
	};

	enum StatusIds {
		STATUS_ALL_ZEROES,
		STATUS_A,
		STATUS_ALL_ONES = STATUS_A + x,
		STATUS_OUT,
		STATUS_A0 = STATUS_OUT + y,
		STATUS_B0 = STATUS_A0 + y,
		STATUS_C0 = STATUS_B0 + y,
		STATUS_D0 = STATUS_C0 + y,
		NUM_STATUS
	};

	status_t statuses[NUM_STATUS] = { 0 };

	DO1() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (unsigned int ix = 0; ix < x; ix++) {
			configParam(PARAM_CONNECTOR_OUT_1 + ix, 0.0f, x + y + 1, 0.0f, "Connection" );
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			configParam(PARAM_GATE_1 + iy, 0.0f, functions.size() - 1.0f, 0.0f, "Gate" );
			configParam(PARAM_CONNECTOR_1 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, "Connection");
			configParam(PARAM_CONNECTOR_2 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, "Connection");
			configParam(PARAM_CONNECTOR_3 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, "Connection");
			configParam(PARAM_CONNECTOR_4 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, "Connection");
		}
		statuses[STATUS_ALL_ZEROES] = 0;
		statuses[STATUS_ALL_ONES] = ~statuses[STATUS_ALL_ZEROES];
		DEBUG("%p", &statuses[STATUS_A0]);
	}
	void process(const ProcessArgs &args) override {
		unsigned int maxPoly = 1;
		for (unsigned int ix = 0; ix < x; ix++) {
			unsigned int channels = inputs[INPUT_1 + ix].getChannels();
			maxPoly = (maxPoly > channels)?maxPoly:channels;
			statuses[STATUS_A + ix] = 0;
			for (unsigned int iy = 0; iy < 16; iy++) {
				statuses[STATUS_A + ix] <<= 1;
				statuses[STATUS_A + ix] += (inputs[INPUT_1 + ix].getPolyVoltage(iy) > midpoint());
			}
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			unsigned int gate = params[PARAM_GATE_1 + iy].getValue();
			if (gate >= functions.size()) {
				gate = functions.size() - 1;
			}
			status_t in[4];
			for (unsigned int ix = 0; ix < 4; ix++) {
				unsigned int val = params[PARAM_CONNECTOR_1 + ix + 4 * iy].getValue();
				if (val > x + iy + 1)
					val = x + iy + 1;
				in[ix] = statuses[STATUS_ALL_ZEROES + val];
			}
			statuses[STATUS_OUT + iy] = functions[gate].process(in[0], in[1], in[2], in[3], statuses[STATUS_A0 + iy], statuses[STATUS_B0 + iy], statuses[STATUS_C0 + iy], statuses[STATUS_D0 + iy]);
		}
		for (unsigned int ix = 0; ix < x; ix++) {
			outputs[OUTPUT_1 + ix].setChannels(maxPoly);
			unsigned int val = params[PARAM_CONNECTOR_OUT_1 + ix].getValue();
			if (val > 1 + x + y)
				val = 1 + x + y;
			for (unsigned int iy = 0; iy < 16; iy++) {
				outputs[OUTPUT_1 + ix].setVoltage(((statuses[STATUS_ALL_ZEROES + val] >> (15-iy)) & 0x1)?voltage1:voltage0, iy);
			}
		}
	}
};

template <unsigned int x, unsigned int y>
struct DOWidget : SchemeModuleWidget {
	ScrollWidget *collectionScrollWidget;
	PLConnectorKnob *knobs[x + 4 * y];
	PLBackground<x,y> *background;
	DOWidget(DO1<x,y> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(x * 30, 380);
		addChild(new SchemePanel(this->box.size));
		background = new PLBackground<x,y>();
		background->box.pos = Vec(5, 45);
		background->box.size = Vec(box.size.x - 10, box.size.y - 90);
		addChild(background);
		collectionScrollWidget = new ScrollWidget();
		float posDiff = background->box.size.x / x;
		float pos = posDiff / 2;
		for (unsigned int ix = 0; ix < x; ix++) {
			knobs[ix + 4 * y] = createParamCentered<PLConnectorKnob>(Vec(pos, background->box.size.y - 5), module, DO1<x, y>::PARAM_CONNECTOR_OUT_1 + ix);
			knobs[ix + 4 * y]->module = module;
			background->addChild(knobs[ix + 4 * y]);
			pos = pos + posDiff;
		}
		collectionScrollWidget->box.pos = Vec(5,55);
		collectionScrollWidget->box.size = Vec(box.size.x - 10, box.size.y - 110);
		addChild(collectionScrollWidget);
		for (unsigned int iy = 0; iy < y; iy++) {
			PLGateKnob *knob = createParamCentered<PLGateKnob>(Vec(53, 80 * (iy + 1)), module, DO1<x,y>::PARAM_GATE_1 + iy);
			knob->module = module;
			knob->index = DO1<x,y>::PARAM_GATE_1 + iy;
			knob->getText = [&knob]()->std::string {
				return this->getGateText(knob);
			};
			collectionScrollWidget->container->addChild(knob);
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			for (unsigned int ix = 0; ix < 4; ix++) {
				knobs[4 * iy + ix] = createParamCentered<PLConnectorKnob>(Vec(5, (iy + 1) * 80.0f + ix * 14.0f - 21.0f), module, DO1<x, y>::PARAM_CONNECTOR_1 + iy * 4 + ix);
				knobs[4 * iy + ix]->module = module;
				collectionScrollWidget->container->addChild(knobs[4 * iy + ix]);	
			}
		}
		PLConnectorRenderer *renderer = new PLConnectorRenderer();
		renderer->box.pos = background->box.pos;
		renderer->box.size = background->box.size;
		renderer->drawLambda = [this](const DrawArgs &args) {
			this->drawConnectors(args);
		};
		addChild(renderer);
		for (unsigned int ix = 0; ix < x; ix++) {
			addInput(createInputCentered<BluePort>(Vec(15 + ix * 30, 30), module, DO1<x,y>::INPUT_1 + ix));
			addOutput(createOutputCentered<BluePort>(Vec(15 + ix * 30, 350), module, DO1<x,y>::OUTPUT_1 + ix));
		}
	}

	std::string getGateText(PLGateKnob *knob) {
		if (!module)
			return std::string("Browser");
		unsigned int val = (unsigned int)APP->engine->getParam(module, knob->index);
		if (val >= functions.size()) {
			val = functions.size() - 1;
		}
		return functions[val].name;
	}

	void drawWire(const DrawArgs &args, float sx, float sy, float dx, float dy, NVGcolor color, float fade) {
		drawConnector(args.vg, sx, sy, color);
		color.a = fade;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, sx, sy);
		nvgLineTo(args.vg, dx, dy);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStrokeColor(args.vg, nvgRGBAf(color.r / 2.0f, color.g / 2.0f, color.b / 2.0f, fade));
		nvgStrokeWidth(args.vg, 3);
		nvgStroke(args.vg);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, 2);
		nvgStroke(args.vg);
	}
	void drawConnectors(const DrawArgs &args) {
		if (!module)
			return;
		for (unsigned int i = 0; i < x; i++) {
			nvgBeginPath(args.vg);
			nvgStrokeWidth(args.vg, 2);
			nvgStrokeColor(args.vg, nvgRGB(255, 255, 255));
			nvgMoveTo(args.vg, (background->box.size.x / (x * 2)) * (i * 2 + 1), background->box.size.y - 5);
			nvgLineTo(args.vg, 15 + i * 30 - background->box.pos.x, 350 - background->box.pos.y);
			nvgStroke(args.vg);
		}
		for (unsigned int i = 0; i < x + 4 * y; i++) {
			float startX = 0;
			float startY = 0;
			float destX = 0;
			float destY = 0;
			float scissorTop = args.clipBox.pos.y;
			float scissorBottom = args.clipBox.size.y;
			if (i < 4 * y) {
				startX = 5;
				startY = (i / 4) * 80.0f + collectionScrollWidget->container->box.pos.y;
				startY += (i % 4) * 14.0f + 69.0f;
				scissorBottom -= 10;
			}
			else {
				startX = (background->box.size.x / (x * 2)) * ((i - 4 * y) * 2 + 1);
				startY = background->box.size.y - 5;
			}
			unsigned int val = (unsigned int)APP->engine->getParam(module, DO1<x,y>::PARAM_CONNECTOR_1 + i);
			if (val > (x + y + 1)) {
				val = (x + y + 1);
			}
			if (val < x + 2) {
				destX = background->box.size.x / (x + 2);
				destX /= 2;
				destX *= (val * 2 + 1);
				destY = 5;
			}
			else {
				destX = 91.0f; 
				destY = 10 + 80 * (val - x - 1) + collectionScrollWidget->container->box.pos.y;
				scissorTop += 10;
				scissorBottom -= 10;
			}
			nvgScissor(args.vg, args.clipBox.pos.x, scissorTop, args.clipBox.size.x, scissorBottom);
			NVGcolor color = nvgRGB(colors[val * 3], colors[val * 3 + 1], colors[val * 3 + 2]);
			float fade = val?knobs[i]->fade:0.0f;
			drawWire(args, startX, startY, destX, destY, color, fade);
			nvgResetScissor(args.vg);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		char workingSpace[10];
		snprintf(workingSpace, 10, "DO-1%02d", y);
		drawBase(vg, workingSpace);
	}
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
		if (dsMod) {
			dsMod->appendContextMenu(menu);
		}
	}
};

Model *modelDO105 = createModel<DO1<4,5>, DOWidget<4,5>>("DO-105");

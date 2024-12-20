//SubTag W10

#include "SubmarineFree.hpp"

struct EN_104 : Module {
	
	enum ParamIds {
		PARAM_A1,
		PARAM_D1 = PARAM_A1 + 4,
		PARAM_S1 = PARAM_D1 + 4,
		PARAM_R1 = PARAM_S1 + 4,
		PARAM_T1 = PARAM_R1 + 4,
		NUM_PARAMS = PARAM_T1 + 4
	};
	enum InputIds {
		INPUT_TRIGGER,
		INPUT_GATE,
		INPUT_1,
		INPUT_A1 = INPUT_1 + 4,
		INPUT_D1 = INPUT_A1 + 4,
		INPUT_S1 = INPUT_D1 + 4,
		INPUT_R1 = INPUT_S1 + 4,
		INPUT_T1 = INPUT_R1 + 4,
		NUM_INPUTS = INPUT_T1 + 4
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = OUTPUT_1 + 4
	};
	enum LightIds {
		LIGHT_A1,
		LIGHT_D1 = LIGHT_A1 + 4,
		LIGHT_S1 = LIGHT_D1 + 4,
		LIGHT_R1 = LIGHT_S1 + 4,
		NUM_LIGHTS = LIGHT_R1 + 4
	};

	EN_104
() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(unsigned int i = 0; i < 4; i++) {
			configParam(PARAM_A1 + i, 0.0f, 1.0f, 0.25f, string::f("Operator #%d Attack Rate", i + 1), " ms", 10000.0f, 1.0f);
			configParam(PARAM_D1 + i, 0.0f, 1.0f, 0.25f, string::f("Operator #%d Decay Rate", i + 1), " ms", 10000.0f, 1.0f);
			configParam(PARAM_S1 + i, 0.0f, 1.0f, 0.8f, string::f("Operator #%d Sustain Level", i + 1), "%", 0.0f, 100.0f);
			configParam(PARAM_R1 + i, 0.0f, 1.0f, 0.25f, string::f("Operator #%d Release Rate", i + 1), " ms", 10000.0f, 1.0f);
			configParam(PARAM_T1 + i, 0.0f, 1.0f, 1.0f, string::f("Operator #%d Total Level", i + 1), "%", 0.0f, 100.0f);
			configInput(INPUT_1 + i, string::f("Operator #%d", i + 1));
			configInput(INPUT_A1 + i, string::f("Operator #%d Attack Rate", i + 1));
			configInput(INPUT_D1 + i, string::f("Operator #%d Decay Rate", i + 1));
			configInput(INPUT_S1 + i, string::f("Operator #%d Sustain Level", i + 1));
			configInput(INPUT_R1 + i, string::f("Operator #%d Release Rate", i + 1));
			configInput(INPUT_T1 + i, string::f("Operator #%d Total Level", i + 1));
			configOutput(OUTPUT_1 + i, string::f("Operator #%d", i + 1));
			configLight(LIGHT_A1 + i, string::f("Operator #%d Attack Phase", i + 1));
			configLight(LIGHT_D1 + i, string::f("Operator #%d Decay Phase", i + 1));
			configLight(LIGHT_S1 + i, string::f("Operator #%d Sustain Phase", i + 1));
			configLight(LIGHT_R1 + i, string::f("Operator #%d Release Phase", i + 1));
		}
		configInput(INPUT_TRIGGER, "Trigger");
		configInput(INPUT_GATE, "Gate");
		
	}
	dsp::SchmittTrigger trigger;
	dsp::SchmittTrigger gate;
	unsigned char skipParams = 0;
	__m128 level = _mm_set_ps1(0.0f);
	__m128 attack;
	__m128 decay;
	__m128 sustain;
	__m128 release;
	__m128 phase = _mm_set_ps1(0.0f);
	__m128 total;
	alignas(16) float a[4];
	alignas(16) float d[4];
	alignas(16) float s[4];
	alignas(16) float r[4];
	alignas(16) float t[4];
	void process(const ProcessArgs &args) override;
	void getParams(const ProcessArgs &args);
};

void EN_104::getParams(const ProcessArgs &args) {
	float delta = args.sampleTime * 0.1f;
	for (unsigned int i = 0; i < 4; i++) {
		a[i] = clamp(params[PARAM_A1 + i].getValue() + inputs[INPUT_A1 + i].getVoltage() * 0.1f, 0.0f, 1.0f);
		a[i] = delta * pow(10000.0f, 1.0f - a[i]);
		d[i] = clamp(params[PARAM_D1 + i].getValue() + inputs[INPUT_D1 + i].getVoltage() * 0.1f, 0.0f, 1.0f);
		d[i] = delta * pow(10000.0f, 1.0f - d[i]);
		s[i] = clamp(params[PARAM_S1 + i].getValue() + inputs[INPUT_S1 + i].getVoltage() * 0.1f, 0.0f, 1.0f);
		r[i] = clamp(params[PARAM_R1 + i].getValue() + inputs[INPUT_R1 + i].getVoltage() * 0.1f, 0.0f, 1.0f);
		r[i] = delta * pow(10000.0f, 1.0f - r[i]);
		t[i] = clamp(params[PARAM_T1 + i].getValue() + inputs[INPUT_T1 + i].getVoltage() * 0.1f, 0.0f, 1.0f);
	}
	attack = _mm_load_ps(a);
	decay = _mm_load_ps(d);
	sustain = _mm_load_ps(s);
	release = _mm_load_ps(r);
	total = _mm_load_ps(t);
	/* // Move exponential from for loop to down here
	attack = _mm_sub_ps(_mm_set_ps1(1.0f), attack);
	decay = _mm_sub_ps(_mm_set_ps1(1.0f), decay);
	release = _mm_sub_ps(_mm_set_ps1(1.0f), release);
	attack = _mm_pow_ps(_mm_set_ps1(10000.0f), attack); // actually implement _mm_pow_ps 
	*/

}

void EN_104::process(const ProcessArgs &args) {
	alignas(16) float v[4];
	alignas(16) float p[4];
	alignas(16) float l[4];
	if (!skipParams++) {
		getParams(args);
	}
	for (int i = 0; i < 4; i++) {
		v[i] = inputs[INPUT_1 + i].getVoltage();
	}
	__m128 voltage = _mm_load_ps(v);
	float triggerVal = inputs[INPUT_TRIGGER].getVoltage();
	float gateVal = inputs[INPUT_GATE].getVoltage();
	bool gated = gateVal > 0.5f;
	unsigned char triggered = trigger.process(rescale(triggerVal, 2.4f, 2.5f, 0.0f, 1.0f));
	if (!inputs[INPUT_TRIGGER].isConnected()) {
		triggered = gate.process(rescale(gateVal, 2.4f, 2.5f, 0.0f, 1.0f));
	}
	if (gated) {
		// Gate is open
		// minGate is the comparison level for sustain, either the sustain level, or current level if already below.
		// set the phase to Attack if triggered
		// add the attack rate if in the Attack phase, else add the decay rate
		// set the phase to decay if we have reached the peak.
		// set the level between 1 and minGate (because the gate is open and we are sustaining)
		__m128 minGate = _mm_min_ps(level, sustain);
		phase = _mm_or_ps(phase, _mm_castsi128_ps(_mm_set1_epi8(triggered * 255)));
		level = _mm_add_ps(level, _mm_and_ps(phase, attack));
		level = _mm_sub_ps(level, _mm_andnot_ps(phase, decay));
		phase = _mm_and_ps(phase, _mm_cmpge_ps(_mm_set_ps1(1.0f), level));
		level = _mm_min_ps(level, _mm_set_ps1(1.0f));
		level = _mm_max_ps(level, _mm_andnot_ps(phase, minGate));
		_mm_store_ps(l, level);
		_mm_store_ps(v, _mm_mul_ps(_mm_mul_ps(level, total), voltage));
		_mm_store_ps(p, phase);
		for (int i = 0; i < 4; i++) {
			lights[LIGHT_A1 + i].setBrightness(p[i] != 0);
			lights[LIGHT_D1 + i].setBrightness((p[i] == 0) && (l[i] > s[i]));
			lights[LIGHT_S1 + i].setBrightness((p[i] == 0) && (l[i] <= s[i]));
			lights[LIGHT_R1 + i].setBrightness(0);
			outputs[OUTPUT_1 + i].setVoltage(v[i]);
		}
	}
	else {
		// Gate is closed
		// set the phase to Attack if triggered
		// add the attack rate if in the Attack phase, else add the release rate
		// set the phase to Decay if we have reached the peak.
		// set the level between 1 and 0.
		phase = _mm_or_ps(phase, _mm_castsi128_ps(_mm_set1_epi8(triggered * 255)));
		level = _mm_add_ps(level, _mm_and_ps(phase, attack));
		level = _mm_sub_ps(level, _mm_andnot_ps(phase, release));
		phase = _mm_and_ps(phase, _mm_cmpge_ps(_mm_set_ps1(1.0f), level));
		level = _mm_min_ps(level, _mm_set_ps1(1.0f));
		level = _mm_max_ps(level, _mm_set_ps1(0.0f));
		_mm_store_ps(v, _mm_mul_ps(_mm_mul_ps(level, total), voltage));
		_mm_store_ps(p, phase);
		for (int i = 0; i < 4; i++) {
			lights[LIGHT_A1 + i].setBrightness(p[i] != 0);
			lights[LIGHT_D1 + i].setBrightness(0);
			lights[LIGHT_S1 + i].setBrightness(0);
			lights[LIGHT_R1 + i].setBrightness(p[i] == 0 && v[i] != 0);
			outputs[OUTPUT_1 + i].setVoltage(v[i]);
		}
	}
}

struct EN104 : SchemeModuleWidget {
	EN104(EN_104 *module) {
		setModule(module);
		this->box.size = Vec(150, 380);
		addChild(new SchemePanel(this->box.size));
		addInput(createInputCentered<SilverPort>(Vec(15, 33), module, EN_104::INPUT_TRIGGER));
		addInput(createInputCentered<SilverPort>(Vec(15, 67), module, EN_104::INPUT_GATE));

		for (int i = 0; i < 4; i++) {
			addInput(createInputCentered<SilverPort>(Vec(45 + 30 * i, 33), module, EN_104::INPUT_1 + i));
			addOutput(createOutputCentered<SilverPort>(Vec(45 + 30 * i, 67), module, EN_104::OUTPUT_1 + i));
			addParam(createParamCentered<SubmarineSmallKnob<SubmarineLightknob>>(Vec(15, 103 + 70 * i), module, EN_104::PARAM_A1 + i));
			addParam(createParamCentered<SubmarineSmallKnob<SubmarineLightknob>>(Vec(45, 103 + 70 * i), module, EN_104::PARAM_D1 + i));
			addParam(createParamCentered<SubmarineSmallKnob<SubmarineLightknob>>(Vec(75, 103 + 70 * i), module, EN_104::PARAM_S1 + i));
			addParam(createParamCentered<SubmarineSmallKnob<SubmarineLightknob>>(Vec(105, 103 + 70 * i), module, EN_104::PARAM_R1 + i));
			addParam(createParamCentered<SubmarineSmallKnob<SubmarineLightknob>>(Vec(135, 103 + 70 * i), module, EN_104::PARAM_T1 + i));
			addInput(createInputCentered<SilverPort>(Vec(15, 137 + 70 * i), module, EN_104::INPUT_A1 + i));
			addInput(createInputCentered<SilverPort>(Vec(45, 137 + 70 * i), module, EN_104::INPUT_D1 + i));
			addInput(createInputCentered<SilverPort>(Vec(75, 137 + 70 * i), module, EN_104::INPUT_S1 + i));
			addInput(createInputCentered<SilverPort>(Vec(105, 137 + 70 * i), module, EN_104::INPUT_R1 + i));
			addInput(createInputCentered<SilverPort>(Vec(135, 137 + 70 * i), module, EN_104::INPUT_T1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(23, 150 + 70 * i), module, EN_104::LIGHT_A1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(53, 150 + 70 * i), module, EN_104::LIGHT_D1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(83, 150 + 70 * i), module, EN_104::LIGHT_S1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(113, 150 + 70 * i), module, EN_104::LIGHT_R1 + i));
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "EN-104");
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);

		for (unsigned int i = 0; i < 4; i++) {
			nvgMoveTo(vg, 3, 85.5 + 70 * i);
			nvgLineTo(vg, 147, 85.5 + 70 * i);
		}

		nvgStroke(vg);
		
		drawText(vg, 2, 20, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "TR.");
		drawText(vg, 2, 54, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "GT.");
		drawText(vg, 45, 54, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "1");
		drawText(vg, 75, 54, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "2");
		drawText(vg, 105, 54, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "3");
		drawText(vg, 135, 54, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "4");
		for (unsigned int i = 0; i < 4; i++) {
			drawText(vg, 15, 124 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "A");
			drawText(vg, 45, 124 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "D");
			drawText(vg, 75, 124 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "S");
			drawText(vg, 105, 124 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "R");
			drawText(vg, 135, 124 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "LVL.");
		}

	}
};

Model *modelEN104 = createModel<EN_104, EN104>("EN-104");

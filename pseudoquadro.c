#include <stdlib.h>
#include <string.h>

#include <ladspa.h>

#define PSEUDOQUADRO_INPUT1            0
#define PSEUDOQUADRO_INPUT2            1
#define PSEUDOQUADRO_OUT1              2
#define PSEUDOQUADRO_OUT2              3
#define PSEUDOQUADRO_OUT3              4
#define PSEUDOQUADRO_OUT4              5
#define PSEUDOQUADRO_GAIN              6

static LADSPA_Descriptor *pseudoquadroDescriptor = NULL;

typedef struct {
	LADSPA_Data *input1;
	LADSPA_Data *input2;
	LADSPA_Data *out1;
	LADSPA_Data *out2;
	LADSPA_Data *out3;
	LADSPA_Data *out4;
	LADSPA_Data *gain;
} Pseudoquadro;


const LADSPA_Descriptor* ladspa_descriptor(unsigned long index) {

	switch (index) {
	case 0:
		return pseudoquadroDescriptor;
	default:
		return NULL;
	}
}

static void cleanupPseudoquadro(LADSPA_Handle instance) {
	free(instance);
}

static void connectPortPseudoquadro(
 LADSPA_Handle instance,
 unsigned long port,
 LADSPA_Data *data) {
	Pseudoquadro *plugin;

	plugin = (Pseudoquadro *)instance;
	switch (port) {
	case PSEUDOQUADRO_INPUT1:
		plugin->input1 = data;
		break;
	case PSEUDOQUADRO_INPUT2:
		plugin->input2 = data;
		break;
	case PSEUDOQUADRO_OUT1:
		plugin->out1 = data;
		break;
	case PSEUDOQUADRO_OUT2:
		plugin->out2 = data;
		break;
	case PSEUDOQUADRO_OUT3:
		plugin->out3 = data;
		break;
	case PSEUDOQUADRO_OUT4:
		plugin->out4 = data;
		break;
	case PSEUDOQUADRO_GAIN:
		plugin->gain = data;
		break;
	}
}

static LADSPA_Handle instantiatePseudoquadro(const LADSPA_Descriptor *descriptor, unsigned long s_rate) {
	Pseudoquadro *plugin_data = (Pseudoquadro *)malloc(sizeof(Pseudoquadro));

	return (LADSPA_Handle)plugin_data;
}

static void runPseudoquadro(LADSPA_Handle instance, unsigned long sample_count) {
	Pseudoquadro *plugin_data = (Pseudoquadro *)instance;

	/* Input 1 (array of floats of length sample_count) */
	const LADSPA_Data * const input1 = plugin_data->input1;

	/* Input 2 (array of floats of length sample_count) */
	const LADSPA_Data * const input2 = plugin_data->input2;

	/* Output 1 (array of floats of length sample_count) */
	LADSPA_Data * const out1 = plugin_data->out1;

	/* Output 2 (array of floats of length sample_count) */
	LADSPA_Data * const out2 = plugin_data->out2;

	/* Output 3 (array of floats of length sample_count) */
	LADSPA_Data * const out3 = plugin_data->out3;

	/* Output 4 (array of floats of length sample_count) */
	LADSPA_Data * const out4 = plugin_data->out4;

	/* rear amplitude (linear [0..100]) */
	const LADSPA_Data gain = *(plugin_data->gain);

	unsigned long pos;
	double coef = (double) gain / 100.0;

	for (pos = 0; pos < sample_count; pos++) {
	        const LADSPA_Data in1 = input1[pos];
	        const LADSPA_Data in2 = input2[pos];

	        out1[pos] = in1;
	        out2[pos] = in2;

	        out3[pos] = coef * (in1/2 - in2/2);
	        out4[pos] = coef * (in2/2 - in1/2);

	}
               
}

static void runaddingPseudoquadro(LADSPA_Handle instance, unsigned long sample_count) {
	Pseudoquadro *plugin_data = (Pseudoquadro *)instance;

	/* Input 1 (array of floats of length sample_count) */
	const LADSPA_Data * const input1 = plugin_data->input1;

	/* Input 2 (array of floats of length sample_count) */
	const LADSPA_Data * const input2 = plugin_data->input2;

	/* Output 1 (array of floats of length sample_count) */
	LADSPA_Data * const out1 = plugin_data->out1;

	/* Output 2 (array of floats of length sample_count) */
	LADSPA_Data * const out2 = plugin_data->out2;

	/* Output 3 (array of floats of length sample_count) */
	LADSPA_Data * const out3 = plugin_data->out3;

	/* Output 4 (array of floats of length sample_count) */
	LADSPA_Data * const out4 = plugin_data->out4;

	/* rear amplitude (linear [0..100]) */
	const LADSPA_Data gain = *(plugin_data->gain);

	unsigned long pos;
	double coef = (double) gain / 100.0;

	for (pos = 0; pos < sample_count; pos++) {
	        const LADSPA_Data in1 = input1[pos];
	        const LADSPA_Data in2 = input2[pos];

	        out1[pos] = in1;
	        out2[pos] = in2;

	        out3[pos] = coef * (in1/2 - in2/2) + out3[pos];
	        out4[pos] = coef * (in2/2 - in1/2) + out4[pos];

	}
               
}

void _init() {
	char **port_names;
	LADSPA_PortDescriptor *port_descriptors;
	LADSPA_PortRangeHint *port_range_hints;

#define D_(s) (s)


	pseudoquadroDescriptor = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

	if (pseudoquadroDescriptor) {
		pseudoquadroDescriptor->UniqueID = 3821;
		pseudoquadroDescriptor->Label = "pseudoquadro";
		pseudoquadroDescriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
		pseudoquadroDescriptor->Name = D_("Stereo to pseudoquadro");
		pseudoquadroDescriptor->Maker = "Markko Merzin";
		pseudoquadroDescriptor->Copyright = "GPL";
		pseudoquadroDescriptor->PortCount = 7;

		port_descriptors = (LADSPA_PortDescriptor *)calloc(7, sizeof(LADSPA_PortDescriptor));
		pseudoquadroDescriptor->PortDescriptors = (const LADSPA_PortDescriptor *)port_descriptors;

		port_range_hints = (LADSPA_PortRangeHint *)calloc(7, sizeof(LADSPA_PortRangeHint));
		pseudoquadroDescriptor->PortRangeHints = (const LADSPA_PortRangeHint *)port_range_hints;

		port_names = (char **)calloc(7, sizeof(char*));
		pseudoquadroDescriptor->PortNames = (const char **)port_names;

		/* Parameters for Input 1 */
		port_descriptors[PSEUDOQUADRO_INPUT1] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[PSEUDOQUADRO_INPUT1] = D_("Input 1");
		port_range_hints[PSEUDOQUADRO_INPUT1].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[PSEUDOQUADRO_INPUT1].LowerBound = -1;
		port_range_hints[PSEUDOQUADRO_INPUT1].UpperBound = +1;

		/* Parameters for Input 2 */
		port_descriptors[PSEUDOQUADRO_INPUT2] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[PSEUDOQUADRO_INPUT2] = D_("Input 2");
		port_range_hints[PSEUDOQUADRO_INPUT2].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[PSEUDOQUADRO_INPUT2].LowerBound = -1;
		port_range_hints[PSEUDOQUADRO_INPUT2].UpperBound = +1;

		/* Parameters for Output 1 */
		port_descriptors[PSEUDOQUADRO_OUT1] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[PSEUDOQUADRO_OUT1] =	 D_("Output 1");
		port_range_hints[PSEUDOQUADRO_OUT1].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[PSEUDOQUADRO_OUT1].LowerBound = -1;
		port_range_hints[PSEUDOQUADRO_OUT1].UpperBound = +1;

		/* Parameters for Output 2 */
		port_descriptors[PSEUDOQUADRO_OUT2] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[PSEUDOQUADRO_OUT2] = D_("Output 2");
		port_range_hints[PSEUDOQUADRO_OUT2].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[PSEUDOQUADRO_OUT2].LowerBound = -1;
		port_range_hints[PSEUDOQUADRO_OUT2].UpperBound = +1;

		/* Parameters for Output 3 */
		port_descriptors[PSEUDOQUADRO_OUT3] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[PSEUDOQUADRO_OUT3] = D_("Output 3");
		port_range_hints[PSEUDOQUADRO_OUT3].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[PSEUDOQUADRO_OUT3].LowerBound = -1;
		port_range_hints[PSEUDOQUADRO_OUT3].UpperBound = +1;

		/* Parameters for Output 4 */
		port_descriptors[PSEUDOQUADRO_OUT4] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[PSEUDOQUADRO_OUT4] = D_("Output 4");
		port_range_hints[PSEUDOQUADRO_OUT4].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[PSEUDOQUADRO_OUT4].LowerBound = -1;
		port_range_hints[PSEUDOQUADRO_OUT4].UpperBound = +1;

		/* Parameters for Rear amplitude */
		port_descriptors[PSEUDOQUADRO_GAIN] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[PSEUDOQUADRO_GAIN] =	 D_("Rear amplitude [0..100]");
		port_range_hints[PSEUDOQUADRO_GAIN].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_0;
		port_range_hints[PSEUDOQUADRO_GAIN].LowerBound = 0;
		port_range_hints[PSEUDOQUADRO_GAIN].UpperBound = 100;

		pseudoquadroDescriptor->activate = NULL;
		pseudoquadroDescriptor->cleanup = cleanupPseudoquadro;
		pseudoquadroDescriptor->connect_port = connectPortPseudoquadro;
		pseudoquadroDescriptor->deactivate = NULL;
		pseudoquadroDescriptor->instantiate = instantiatePseudoquadro;
		pseudoquadroDescriptor->run = runPseudoquadro;
		pseudoquadroDescriptor->run_adding = runaddingPseudoquadro;
		pseudoquadroDescriptor->set_run_adding_gain = NULL;
	}
}

void _fini() {
	if (pseudoquadroDescriptor) {
		free((LADSPA_PortDescriptor *)pseudoquadroDescriptor->PortDescriptors);
		free((char **)pseudoquadroDescriptor->PortNames);
		free((LADSPA_PortRangeHint *)pseudoquadroDescriptor->PortRangeHints);
		free(pseudoquadroDescriptor);
	}

}




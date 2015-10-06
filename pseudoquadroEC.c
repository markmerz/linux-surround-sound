#include <stdlib.h>
#include <string.h>

#include <ladspa.h>

#define PSEUDOQUADRO_INPUT1            0
#define PSEUDOQUADRO_INPUT2            1
#define PSEUDOQUADRO_OUT1              2
#define PSEUDOQUADRO_OUT2              3
#define PSEUDOQUADRO_OUT3              4
#define PSEUDOQUADRO_OUT4              5
#define PSEUDOQUADRO_OUT5              6
#define PSEUDOQUADRO_REARGAIN              7
#define PSEUDOQUADRO_CENTREGAIN            8

static LADSPA_Descriptor *pseudoquadroDescriptor = NULL;

typedef struct {
	LADSPA_Data *input1;
	LADSPA_Data *input2;
	LADSPA_Data *out1;
	LADSPA_Data *out2;
	LADSPA_Data *out3;
	LADSPA_Data *out4;
	LADSPA_Data *out5;
	LADSPA_Data *reargain;
	LADSPA_Data *centregain;
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
	case PSEUDOQUADRO_OUT5:
		plugin->out5 = data;
		break;
	case PSEUDOQUADRO_REARGAIN:
		plugin->reargain = data;
		break;
	case PSEUDOQUADRO_CENTREGAIN:
		plugin->centregain = data;
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

	/* Output 5 (array of floats of length sample_count) */
	LADSPA_Data * const out5 = plugin_data->out5;

	/* rear amplitude (linear [0..100]) */
	const LADSPA_Data reargain = *(plugin_data->reargain);

	/* centre amplitude (linear [0..100]) */
	const LADSPA_Data centregain = *(plugin_data->centregain);

	unsigned long pos;
	double coef = (double) reargain / 100.0;
	double coef2 = (double) centregain / 100.0;

	for (pos = 0; pos < sample_count; pos++) {
	        const LADSPA_Data in1 = input1[pos];
	        const LADSPA_Data in2 = input2[pos];

	        out1[pos] = in1;
	        out2[pos] = in2;

	        out3[pos] = coef * (in1/2 - in2/2);
	        out4[pos] = coef * (in2/2 - in1/2);

		out5[pos] = (-in1 - in2) * coef2;

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

	/* Output 5 (array of floats of length sample_count) */
	LADSPA_Data * const out5 = plugin_data->out5;

	/* rear amplitude (linear [0..100]) */
	const LADSPA_Data reargain = *(plugin_data->reargain);

	/* centre amplitude (linear [0..100]) */
	const LADSPA_Data centregain = *(plugin_data->centregain);

	unsigned long pos;
	double coef = (double) reargain / 100.0;
	double coef2 = (double) centregain / 100.0;

	for (pos = 0; pos < sample_count; pos++) {
	        const LADSPA_Data in1 = input1[pos];
	        const LADSPA_Data in2 = input2[pos];

	        out1[pos] = in1;
	        out2[pos] = in2;

	        out3[pos] = coef * (in1/2 - in2/2) + out3[pos];
	        out4[pos] = coef * (in2/2 - in1/2) + out4[pos];

		out5[pos] = (-in1 - in2) * coef2 + out5[pos];

	}
               
}

void _init() {
	char **port_names;
	LADSPA_PortDescriptor *port_descriptors;
	LADSPA_PortRangeHint *port_range_hints;

#define D_(s) (s)


	pseudoquadroDescriptor = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

	if (pseudoquadroDescriptor) {
		pseudoquadroDescriptor->UniqueID = 3823;
		pseudoquadroDescriptor->Label = "pseudoquadroEC";
		pseudoquadroDescriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
		pseudoquadroDescriptor->Name = D_("Stereo to pseudoquadro with centre stereobase expander");
		pseudoquadroDescriptor->Maker = "Markko Merzin";
		pseudoquadroDescriptor->Copyright = "GPL";
		pseudoquadroDescriptor->PortCount = 9;

		port_descriptors = (LADSPA_PortDescriptor *)calloc(9, sizeof(LADSPA_PortDescriptor));
		pseudoquadroDescriptor->PortDescriptors = (const LADSPA_PortDescriptor *)port_descriptors;

		port_range_hints = (LADSPA_PortRangeHint *)calloc(9, sizeof(LADSPA_PortRangeHint));
		pseudoquadroDescriptor->PortRangeHints = (const LADSPA_PortRangeHint *)port_range_hints;

		port_names = (char **)calloc(9, sizeof(char*));
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

		/* Parameters for Output 5 */
		port_descriptors[PSEUDOQUADRO_OUT5] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[PSEUDOQUADRO_OUT5] = D_("Output 5");
		port_range_hints[PSEUDOQUADRO_OUT5].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[PSEUDOQUADRO_OUT5].LowerBound = -1;
		port_range_hints[PSEUDOQUADRO_OUT5].UpperBound = +1;

		/* Parameters for Rear amplitude */
		port_descriptors[PSEUDOQUADRO_REARGAIN] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[PSEUDOQUADRO_REARGAIN] =	 D_("Rear amplitude [0..100]");
		port_range_hints[PSEUDOQUADRO_REARGAIN].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_0;
		port_range_hints[PSEUDOQUADRO_REARGAIN].LowerBound = 0;
		port_range_hints[PSEUDOQUADRO_REARGAIN].UpperBound = 100;

		/* Parameters for Centre amplitude */
		port_descriptors[PSEUDOQUADRO_CENTREGAIN] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[PSEUDOQUADRO_CENTREGAIN] =	 D_("Centre amplitude [0..100]");
		port_range_hints[PSEUDOQUADRO_CENTREGAIN].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_0;
		port_range_hints[PSEUDOQUADRO_CENTREGAIN].LowerBound = 0;
		port_range_hints[PSEUDOQUADRO_CENTREGAIN].UpperBound = 100;

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




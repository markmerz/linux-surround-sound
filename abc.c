#include <stdlib.h>
#include <string.h>

#include <ladspa.h>

#define ABC_INPUT1            0
#define ABC_INPUT2            1
#define ABC_OUT1              2
#define ABC_OUT2              3
#define ABC_OUT3              4
#define ABC_OUT4              5
#define ABC_GAIN              6

static LADSPA_Descriptor *abcDescriptor = NULL;

typedef struct {
	LADSPA_Data *input1;
	LADSPA_Data *input2;
	LADSPA_Data *out1;
	LADSPA_Data *out2;
	LADSPA_Data *out3;
	LADSPA_Data *out4;
	LADSPA_Data *gain;
	LADSPA_Data *addinggain;
} Abc;


const LADSPA_Descriptor* ladspa_descriptor(unsigned long index) {

	switch (index) {
	case 0:
		return abcDescriptor;
	default:
		return NULL;
	}
}

static void cleanupAbc(LADSPA_Handle instance) {
	free(instance);
}

static void connectPortAbc(
 LADSPA_Handle instance,
 unsigned long port,
 LADSPA_Data *data) {
	Abc *plugin;

	plugin = (Abc *)instance;
	switch (port) {
	case ABC_INPUT1:
		plugin->input1 = data;
		break;
	case ABC_INPUT2:
		plugin->input2 = data;
		break;
	case ABC_OUT1:
		plugin->out1 = data;
		break;
	case ABC_OUT2:
		plugin->out2 = data;
		break;
	case ABC_OUT3:
		plugin->out3 = data;
		break;
	case ABC_OUT4:
		plugin->out4 = data;
		break;
	case ABC_GAIN:
		plugin->gain = data;
		break;
	}
}

static LADSPA_Handle instantiateAbc(const LADSPA_Descriptor *descriptor, unsigned long s_rate) {
	Abc *plugin_data = (Abc *)malloc(sizeof(Abc));

	return (LADSPA_Handle)plugin_data;
}

static void runAbc(LADSPA_Handle instance, unsigned long sample_count) {
	Abc *plugin_data = (Abc *)instance;

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

	        out3[pos] = coef * (in1/2 - 0.7 * in2/2);
	        out4[pos] = coef * (in2/2 - 0.7 * in1/2);

	}
               
}

static void runaddingAbc(LADSPA_Handle instance, unsigned long sample_count) {
	Abc *plugin_data = (Abc *)instance;

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

	const LADSPA_Data addinggain = *(plugin_data->addinggain);

	unsigned long pos;
	double coef = (double) gain / 100.0;

	double coef2 = (double) addinggain;

	for (pos = 0; pos < sample_count; pos++) {
	        const LADSPA_Data in1 = input1[pos];
	        const LADSPA_Data in2 = input2[pos];

	        out1[pos] = in1;
	        out2[pos] = in2;

	        out3[pos] = coef * coef2 * (in1/2 - 0.7 * in2/2) + out3[pos];
	        out4[pos] = coef * coef2 * (in2/2 - 0.7 * in1/2) + out4[pos];

	}
               
}

static void set_run_adding_gain(LADSPA_Handle instance, LADSPA_Data new_gain) {
	Abc *plugin_data = (Abc *)instance;
	*(plugin_data->addinggain) = new_gain;
}

void _init() {
	char **port_names;
	LADSPA_PortDescriptor *port_descriptors;
	LADSPA_PortRangeHint *port_range_hints;

#define D_(s) (s)


	abcDescriptor = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

	if (abcDescriptor) {
		abcDescriptor->UniqueID = 3822;
		abcDescriptor->Label = "abc";
		abcDescriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
		abcDescriptor->Name = D_("Stereo to abc");
		abcDescriptor->Maker = "Markko Merzin";
		abcDescriptor->Copyright = "GPL";
		abcDescriptor->PortCount = 7;

		port_descriptors = (LADSPA_PortDescriptor *)calloc(7, sizeof(LADSPA_PortDescriptor));
		abcDescriptor->PortDescriptors = (const LADSPA_PortDescriptor *)port_descriptors;

		port_range_hints = (LADSPA_PortRangeHint *)calloc(7, sizeof(LADSPA_PortRangeHint));
		abcDescriptor->PortRangeHints = (const LADSPA_PortRangeHint *)port_range_hints;

		port_names = (char **)calloc(7, sizeof(char*));
		abcDescriptor->PortNames = (const char **)port_names;

		/* Parameters for Input 1 */
		port_descriptors[ABC_INPUT1] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[ABC_INPUT1] = D_("Input 1");
		port_range_hints[ABC_INPUT1].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[ABC_INPUT1].LowerBound = -1;
		port_range_hints[ABC_INPUT1].UpperBound = +1;

		/* Parameters for Input 2 */
		port_descriptors[ABC_INPUT2] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[ABC_INPUT2] = D_("Input 2");
		port_range_hints[ABC_INPUT2].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[ABC_INPUT2].LowerBound = -1;
		port_range_hints[ABC_INPUT2].UpperBound = +1;

		/* Parameters for Output 1 */
		port_descriptors[ABC_OUT1] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[ABC_OUT1] =	 D_("Output 1");
		port_range_hints[ABC_OUT1].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[ABC_OUT1].LowerBound = -1;
		port_range_hints[ABC_OUT1].UpperBound = +1;

		/* Parameters for Output 2 */
		port_descriptors[ABC_OUT2] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[ABC_OUT2] = D_("Output 2");
		port_range_hints[ABC_OUT2].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[ABC_OUT2].LowerBound = -1;
		port_range_hints[ABC_OUT2].UpperBound = +1;

		/* Parameters for Output 3 */
		port_descriptors[ABC_OUT3] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[ABC_OUT3] = D_("Output 3");
		port_range_hints[ABC_OUT3].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[ABC_OUT3].LowerBound = -1;
		port_range_hints[ABC_OUT3].UpperBound = +1;

		/* Parameters for Output 4 */
		port_descriptors[ABC_OUT4] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[ABC_OUT4] = D_("Output 4");
		port_range_hints[ABC_OUT4].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[ABC_OUT4].LowerBound = -1;
		port_range_hints[ABC_OUT4].UpperBound = +1;

		/* Parameters for Rear amplitude */
		port_descriptors[ABC_GAIN] = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[ABC_GAIN] =	 D_("Rear amplitude [0..100]");
		port_range_hints[ABC_GAIN].HintDescriptor = LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_0;
		port_range_hints[ABC_GAIN].LowerBound = 0;
		port_range_hints[ABC_GAIN].UpperBound = 100;

		abcDescriptor->activate = NULL;
		abcDescriptor->cleanup = cleanupAbc;
		abcDescriptor->connect_port = connectPortAbc;
		abcDescriptor->deactivate = NULL;
		abcDescriptor->instantiate = instantiateAbc;
		abcDescriptor->run = runAbc;
		abcDescriptor->run_adding = runaddingAbc;
		abcDescriptor->set_run_adding_gain = set_run_adding_gain;
	}
}

void _fini() {
	if (abcDescriptor) {
		free((LADSPA_PortDescriptor *)abcDescriptor->PortDescriptors);
		free((char **)abcDescriptor->PortNames);
		free((LADSPA_PortRangeHint *)abcDescriptor->PortRangeHints);
		free(abcDescriptor);
	}

}




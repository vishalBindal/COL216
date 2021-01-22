#include "global.h"
#include "Instruction.h"
#include "UtilFunctions.h"
#include "Instruction_helper.h"

bool debug = false;
bool syntax_input = true;

int main() {
    // initialise register file
    for (int &regi : reg_array)
        regi = 0;
    reg_array[0] = 2;
    reg_array[1] = 1;
    reg_array[29] = 4095;
    display_register_file();

    if(syntax_input) {
        // parse input file and generate a 'clean' input file
        generate_clean_input_file();

        // parse the clean input file, identify labels
        // convert instructions to 32-bit strings and store in file
        print_binary_instructions_to_file();
    }

    // read binary instructions from file and fill memory
    fill_instructions_in_memory();

    int total_cycles = 0;

    // start the processor!
    cout << "\n-- Clock cycle - " << total_cycles + 1 << "\n\n";
    for (int i = 5; i >= 2; --i)
        display_empty(i);
    instruction_fetch();
    total_cycles++;
    display_register_file();
    while (!(pipeline.empty())) {

        if (debug) getchar();
        cout << "\n-- Clock cycle - " << total_cycles + 1 << "\n\n";

        if (stalled) {
            --stall_cycles;
            if (stall_cycles == 0)
                stalled = false;
        }

        int pipeline_stage = 5;
        int pipe_size = pipeline.size();
        for (int i = 0; i < pipe_size; ++i) {
            while (pipeline[i]->stage != pipeline_stage) {
                display_empty(pipeline_stage);
                --pipeline_stage;
            }
            instruction_perform(i, pipe_size);
            --pipeline_stage;
        }
        while (pipeline_stage >= 2) {
            display_empty(pipeline_stage);
            --pipeline_stage;
        }

        if (!stalled) {
            if (!instruction_fetch())
                display_empty(1);
        } else
            cout << "IF : <STALLED>\n";

        ++total_cycles;
        display_register_file();
    }

    cout << endl << "Total cycles: " << total_cycles << endl;
    cout << "Average instructions per cycle(IPC) : " << (float) ((float) total_instr / (float) total_cycles) << endl;

    return 0;
}

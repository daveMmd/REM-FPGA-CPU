/*
 * Copyright (c) 2007 Michela Becchi and Washington University in St. Louis.
 * All rights reserved
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. The name of the author or Washington University may not be used
 *       to endorse or promote products derived from this source code
 *       without specific prior written permission.
 *    4. Conditions of any other entities that contributed to this are also
 *       met. If a copyright notice is present from another entity, it must
 *       be maintained in redistributions of the source code.
 *
 * THIS INTELLECTUAL PROPERTY (WHICH MAY INCLUDE BUT IS NOT LIMITED TO SOFTWARE,
 * FIRMWARE, VHDL, etc) IS PROVIDED BY  THE AUTHOR AND WASHINGTON UNIVERSITY
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR WASHINGTON UNIVERSITY
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS INTELLECTUAL PROPERTY, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * */

/*
 * File:   main.c
 * Author: Michela Becchi
 * Email:  mbecchi@cse.wustl.edu
 * Organization: Applied Research Laboratory
 *
 * Description: This is the main entry file
 *
 */

#include "stdinc.h"
#include "nfa.h"
#include "dfa.h"
#include "ecdfa.h"
#include "hybrid_fa.h"
#include "parser.h"
#include "trace.h"
#include "component_tree/util.h"
#include "hierarMerging.h"

/*
 * Program entry point.
 * Please modify the main() function to add custom code.
 * The options allow to create a DFA from a list of regular expressions.
 * If a single single DFA cannot be created because state explosion occurs, then a list of DFA
 * is generated (see MAX_DFA_SIZE in dfa.h).
 * Additionally, the DFA can be exported in proprietary format for later re-use, and be imported.
 * Moreover, export to DOT format (http://www.graphviz.org/) is possible.
 * Finally, processing a trace file is an option.
 */


#ifndef CUR_VER
#define CUR_VER		"Michela  Becchi 1.4.1"
#endif

int VERBOSE;
int DEBUG;

/*
 * Returns the current version string
 */
void version(){
    printf("version:: %s\n", CUR_VER);
}

/* usage */
static void usage()
{
    fprintf(stderr,"\n");
    fprintf(stderr, "Usage: regex [options]\n");
    fprintf(stderr, "             [--parse|-p <regex_file> [--m|--i] | --import|-i <in_file> ]\n");
    fprintf(stderr, "             [--export|-e  <out_file>][--graph|-g <dot_file>]\n");
    fprintf(stderr, "             [--trace|-t <trace_file>]\n");
    fprintf(stderr, "             [--hfa]\n\n");
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "    --help,-h       print this message\n");
    fprintf(stderr, "    --version,-r    print version number\n");
    fprintf(stderr, "    --verbose,-v    basic verbosity level \n");
    fprintf(stderr, "    --debug,  -d    enhanced verbosity level \n");
    fprintf(stderr, "\nOther:\n");
    fprintf(stderr, "    --parse,-p <regex_file>  process regex file\n");
    fprintf(stderr, "    --m,--i  m modifier, ignore case\n");
    fprintf(stderr, "    --import,-i <in_file>    import DFA from file\n");
    fprintf(stderr, "    --export,-e <out_file>   export DFA to file\n");
    fprintf(stderr, "    --graph,-g <dot_file>    export DFA in DOT format into specified file\n");
    fprintf(stderr, "    --trace,-t <trace_file>  trace file to be processed\n");
    fprintf(stderr, "    --hfa                    generate the hybrid-FA\n");
    fprintf(stderr, "\n");
    exit(0);
}

/* configuration */
static struct conf {
    char *regex_file;
    char *hex_regex_file;
    char *in_file;
    char *out_file;
    char *dot_file;
    char *trace_file;
    bool i_mod;
    bool m_mod;
    bool verbose;
    bool debug;
    bool hfa;
} config;

/* initialize the configuration */
void init_conf(){
    config.regex_file=NULL;
    config.in_file=NULL;
    config.out_file=NULL;
    config.dot_file=NULL;
    config.trace_file=NULL;
    config.i_mod=false;
    config.m_mod=false;
    config.debug=false;
    config.verbose=false;
    config.hfa=false;
}

/* print the configuration */
void print_conf(){
    fprintf(stderr,"\nCONFIGURATION: \n");
    if (config.regex_file) fprintf(stderr, "- RegEx file: %s\n",config.regex_file);
    if (config.in_file) fprintf(stderr, "- DFA import file: %s\n",config.in_file);
    if (config.out_file) fprintf(stderr, "- DFA export file: %s\n",config.out_file);
    if (config.dot_file) fprintf(stderr, "- DOT file: %s\n",config.dot_file);
    if (config.trace_file) fprintf(stderr,"- Trace file: %s\n",config.trace_file);
    if (config.i_mod) fprintf(stderr,"- ignore case selected\n");
    if (config.m_mod) fprintf(stderr,"- m modifier selected\n");
    if (config.verbose && !config.debug) fprintf(stderr,"- verbose mode\n");
    if (config.debug) fprintf(stderr,"- debug mode\n");
    if (config.hfa)   fprintf(stderr,"- hfa generation invoked\n");
}

/* parse the main call parameters */
static int parse_arguments(int argc, char **argv)
{
    int i=1;
    if (argc < 2) {
        usage();
        return 0;
    }
    while(i<argc){
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            usage();
            return 0;
        }else if(strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--version") == 0){
            version();
            return 0;
        }else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
            config.verbose=1;
        }else if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0){
            config.debug=1;
        }else if(strcmp(argv[i], "--hfa") == 0){
            config.hfa=1;
        }else if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--graph") == 0){
            i++;
            if(i==argc){
                fprintf(stderr,"Dot file name missing.\n");
                return 0;
            }
            config.dot_file=argv[i];
        }else if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--import") == 0){
            i++;
            if(i==argc){
                fprintf(stderr,"Import file name missing.\n");
                return 0;
            }
            config.in_file=argv[i];
        }else if(strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--export") == 0){
            i++;
            if(i==argc){
                fprintf(stderr,"Export file name missing.\n");
                return 0;
            }
            config.out_file=argv[i];
        }else if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--parse") == 0){
            i++;
            if(i==argc){
                fprintf(stderr,"Regular expression file name missing.\n");
                return 0;
            }
            config.regex_file=argv[i];
        }else if(strcmp(argv[i], "--hexp") == 0){
            i++;
            if(i==argc){
                fprintf(stderr,"Hex regular expression file name missing.\n");
                return 0;
            }
            config.hex_regex_file=argv[i];
        }
        else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--trace") == 0){
            i++;
            if(i==argc){
                fprintf(stderr,"Trace file name missing.\n");
                return 0;
            }
            config.trace_file=argv[i];
        }else if(strcmp(argv[i], "--m") == 0){
            config.m_mod=true;
        }else if(strcmp(argv[i], "--i") == 0){
            config.i_mod=true;
        }else{
            fprintf(stderr,"Ignoring invalid option %s\n",argv[i]);
        }
        i++;
    }
    return 1;
}

/* check that the given file can be read/written */
void check_file(char *filename, char *mode){
    FILE *file=fopen(filename,mode);
    if (file==NULL){
        fprintf(stderr,"Unable to open file %s in %c mode",filename,mode);
        fatal("\n");
    }else fclose(file);
}


void test_outtrans(DFA *anchor_dfa){
    int degree = 256;
    for(symbol_t c = 0; c < CSIZE; c++ ){
        if(anchor_dfa->get_next_state(anchor_dfa->initial_state, c) == anchor_dfa->dead_state){
            degree--;
        }
    }
    printf("0 state out degree: %d\n", degree);
}
/*
 *  MAIN - entry point
 */
int main(int argc, char **argv){

    //read configuration
    init_conf();
    while(!parse_arguments(argc,argv)) usage();
    print_conf();
    VERBOSE=config.verbose;
    DEBUG=config.debug; if (DEBUG) VERBOSE=1;

    //check that it is possible to open the files
    if (config.regex_file!=NULL) check_file(config.regex_file,"r");
    if (config.in_file!=NULL) check_file(config.in_file,"r");
    if (config.out_file!=NULL) check_file(config.out_file,"w");
    if (config.dot_file!=NULL) check_file(config.dot_file,"w");
    if (config.trace_file!=NULL) check_file(config.trace_file,"r");

    // check that either a regex file or a DFA import file are given as input
    if (config.regex_file==NULL && config.hex_regex_file== nullptr && config.in_file==NULL){
        fatal("No data file - please use either a regex or a DFA import file\n");
    }
    if (config.regex_file!=NULL && config.in_file!=NULL){
        printf("DFA will be imported from the Regex file. Import file will be ignored");
    }

    /* FA declaration */
    NFA *nfa=NULL;  	// NFA
    DFA *dfa=NULL;		// DFA
    DFA* anchor_dfa = nullptr;

    // if regex file is provided, parses it and instantiate the corresponding NFA.
    // if feasible, convert the NFA to DFA

    if (config.regex_file!=NULL){
        list<char *>* regex_list = read_regexset(config.regex_file);
        //list<char *>* regex_list = read_regexset_hex(config.regex_file);
        auto parser = regex_parser(false, false);
        auto *dfalis = new list<DFA*>();

#if 1//??????DFA/AC
        for(auto &re: *regex_list){
            NFA* nfa = parser.parse_from_regex(re);
            DFA* dfa = nfa->nfa2dfa();
            dfa->minimize();
            dfalis->push_back(dfa);
        }
        dfa = hm_dfalist2dfa(dfalis);
        printf("dfa size: %d\n", dfa->size());

        dfalis = new list<DFA*>();
#endif
        //??????PFDFA / PFAC

        int cnt = 0;
        for(auto &re: *regex_list) {
            cnt++;
            char *anchor_re = (char *) malloc(sizeof(char) * (1000 + 5));
            if(re[0] == '^'){
                strcpy(anchor_re, re);
            }
            else{
                sprintf(anchor_re, "^%s", re);
            }
            printf("cnt:%d hexs:%s anchor_re:%s\n", cnt, re, anchor_re);
            NFA* nfa = parser.parse_from_regex(anchor_re);
            DFA* dfa = nfa->nfa2dfa();
            dfa->minimize();
            dfalis->push_back(dfa);
        }

        for(auto &dfai: *dfalis){
            if(dfai->dead_state == NO_STATE){
                fprintf(stderr, "dfa no dead_state!\n");
            }
            printf("dfa dead_state:%u\n", dfai->dead_state);
        }

        anchor_dfa = hm_dfalist2dfa(dfalis);

        anchor_dfa->renumber();

        printf("anchor_dfa->max_accept_state: %u\n", anchor_dfa->max_accept_state);

        printf("anchor_dfa size: %d\n", anchor_dfa->size());
    }
    else if(config.hex_regex_file != nullptr){
        //list<char *>* regex_list = read_regexset_hex(config.hex_regex_file);
        list<char *>* regex_list = read_regexset(config.hex_regex_file);
        auto parser = regex_parser(false, false);
        auto *dfalis = new list<DFA*>();

#if 1//??????DFA/AC
        for(auto &re: *regex_list){
            if(strlen(re) < 2) {
                fprintf(stderr, "len(re) < 2!\n");
                continue;
            }
            NFA* nfa = parser.string2nfa(re);
            DFA* dfa = nfa->nfa2dfa();
            /*FILE* file = fopen("dfa.dot", "w");
            dfa->to_dot(file, "dfa");
            fclose(file);*/
            dfa->minimize();
            dfalis->push_back(dfa);
        }
        dfa = hm_dfalist2dfa(dfalis);
        printf("dfa size: %d\n", dfa->size());

        //exit(0);
        dfalis = new list<DFA*>();
#endif
        //??????PFDFA / PFAC
        for(auto &re: *regex_list) {
            if(strlen(re) < 2) {
                fprintf(stderr, "len(re) < 2!\n");
                continue;
            }
            NFA* nfa = parser.string2nfa(re, true);
            DFA* dfa = nfa->nfa2dfa();
            dfa->minimize();
            dfalis->push_back(dfa);
        }

        for(auto &dfai: *dfalis){
            if(dfai->dead_state == NO_STATE){
                fprintf(stderr, "dfa no dead_state!\n");
            }
            //printf("dfa dead_state:%u\n", dfai->dead_state);
        }

        anchor_dfa = hm_dfalist2dfa(dfalis);

        anchor_dfa->renumber();

        printf("anchor_dfa->max_accept_state: %u\n", anchor_dfa->max_accept_state);

        printf("anchor_dfa size: %d\n", anchor_dfa->size());
    }

    // if a regex file is not provided, import the DFA
    if (config.regex_file==NULL && config.in_file!=NULL){
        FILE *in_file=fopen(config.in_file,"r");
        fprintf(stderr,"\nImporting from file %s ...\n",config.in_file);
        dfa=new DFA();
        dfa->get(in_file);
        fclose(in_file);
    }

    // DFA export
    if (dfa!=NULL && config.out_file!=NULL){
        FILE *out_file=fopen(config.out_file,"w");
        fprintf(stderr,"\nExporting to file %s ...\n",config.out_file);
        dfa->put(out_file);
        fclose(out_file);
    }

    // DOT file generation
    if (dfa!=NULL && config.dot_file!=NULL){
        FILE *dot_file=fopen(config.dot_file,"w");
        fprintf(stderr,"\nExporting to DOT file %s ...\n",config.dot_file);
        char string[100];
        if (config.regex_file!=NULL) sprintf(string,"source: %s",config.regex_file);
        else sprintf(string,"source: %s",config.in_file);
        dfa->to_dot(dot_file, string);
        fclose(dot_file);
    }

    // trace file traversal
    if (config.trace_file){
        trace *tr=new trace(config.trace_file);
        if (dfa!=NULL){
            tr->traverse_test_DFA_speed(dfa, 1);
            //tr->traverse(dfa);
            //if (dfa->get_default_tx()!=NULL) tr->traverse_compressed(dfa);
        }
        if(anchor_dfa != nullptr){
            //test PFAC outtransitions
            test_outtrans(anchor_dfa);
            tr->traverse_test_PFAC_speed(anchor_dfa, 1);
            tr->traverse_test_PFAC_speed_DFC_improve(anchor_dfa, 1);
            //tr->traverse_test_PFAC_speed2(anchor_dfa, 1);
        }
        delete tr;
    }

    /* Automata de-allocation */

    if (nfa!=NULL) delete nfa;
    if (dfa!=NULL) delete dfa;

    return 0;

}


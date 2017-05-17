import sys
import os
import shutil
import subprocess
import argparse
import random
import json
from pprint import pprint
#Directory Source files should be copied from
SETUP_CODE_DIR = '.'
#Name of directory in each file where Src is locate
SOURCE_DIR_NAME = "."
SOURCE_ROOT="../src/"
#files that should be copied from SETUP_CODE_DIR/ to project_dir/
ROOT_FILES_TO_COPY = ['makefile']

#files that should be copied from SETUP_CODE_DIR/ to project_dir/Src
SRC_FILES_TO_COPY = ['init.c']
#List of directories that should be excluded from the make

#Excluding miniz because it tries to malloc 300000 bytes
MAKE_EXCLUDE = ['../src/platformcode', '../src/miniz']
#Excluding all those that don't use safestack
'''
MAKE_EXCLUDE = ['../src/2dfir',
'../src/adpcm',
'../src/aha_compress',
'../src/bs',
'../src/bubblesort',
'../src/cnt',
'../src/compress',
'../src/cover',
'../src/crc',
'../src/crc32',
'../src/ctl',
'../src/ctl_string',
'../src/dijkstra',
'../src/duff',
'../src/expint',
'../src/fac',
'../src/fasta',
'../src/fdct',
'../src/fft',
'../src/fibcall',
'../src/insertsort',
'../src/janne_complex',
'../src/jfdctint',
'../src/lcdnum',
'../src/lms',
'../src/main.c',
'../src/matmult',
'../src/matmult_float',
'../src/matmult_int',
'../src/nettle_arcfour',
'../src/nettle_cast128',
'../src/nettle_des',
'../src/newlib_exp',
'../src/newlib_log',
'../src/newlib_mod',
'../src/newlib_sqrt',
'../src/ns',
'../src/nsichneu',
'../src/platformcode',
'../src/qrduino',
'../src/qsort',
'../src/qurt',
'../src/recursion',
'../src/select',
'../src/sglib_arraybinsearch',
'../src/sglib_arrayheapsort',
'../src/sglib_dllist',
'../src/sqrt',
'../src/statemate',
'../src/stb_perlin',
'../src/strstr',
'../src/tarai',
'../src/template',
'../src/trio',
'../src/huffbench',  # here and below have safestack but don't run
'../src/miniz',
'../src/trio_sscanf',
'../src/levenshtein']
'''

SIZE_TEXT = 'text'
SIZE_DATA = 'data'
SIZE_BSS = 'bss'
SIZE_TOTAL = 'total'
SIZE_STACK = 'Safe'
SIZE_UNSAFE = 'Unsafe'


def setup_project(project_dir):
    '''
    Setup project take the example projects from ST and setups
    them up for building with multicompiler
    Requirements:
        Global Variables should be setup above
    '''

    print "="*80
    print "PROJECT: ",project_dir
    #Delete Extra Project Files
    print "-"*80
    print "-"*80
    print "Coping Files"
    for f in ROOT_FILES_TO_COPY:
        source_name = os.path.join(SETUP_CODE_DIR,f)
        dest_name = os.path.join(project_dir,f)
        print "\tCopying: %s to %s"%(source_name,dest_name)
        subprocess.call(['cp',source_name,dest_name])
    print "="*80

def gen_builds(num_random,global_padding=12,funct_padding=0,options={}):
    '''
    Calls make on every subdirectory in of the current directory except
    those listed in the MAKE_EXCLUDE global variable
    Requirements:
        Global Variables above should be setup above
        Projects should be subdirectories of the current directory
    '''

    random.seed(0)
    builds ={'Baseline':['make','-j16'],
             'SafeStack':['make','-j16','SAFESTACK=1'],
             'Reduce_Priv':['make','-j16','REDUCE_PRIV=1'],
             'Rand_Regs':['make','-j16','RAND_REGS=1'],
             'Function_Rand':['make','-j16','FUNCTION_PADDING=200'],
             'Data_Rand':['make','-j16','GLOBAL_PADDING=200'],
             'SafeStack-Reduce_Priv':['make','-j16','REDUCE_PRIV=1','SAFESTACK=1']
             }


    for idx in xrange(num_random):
        rand_num = random.randint(0,0xFFFFFFFF)
        #print "Making: %s/build/%s-%i.elf"%(project_dir,project_dir,rand_num)
        make_argv=['make','-j16', 'RAND_SEED=%i'%rand_num]
        make_argv.append('GLOBAL_PADDING=%s'%global_padding)
        make_argv.append('FUNCTION_PADDING=%s'%funct_padding)
        make_argv.append('SAFESTACK=1')
        if not('no_regs' in options.keys() and options['no_regs']):
            print "Removing Rand Regs"
            make_argv.append('RAND_REGS=1')
        make_argv.append('REDUCE_PRIV=REDUCE_PRIV=1')
        builds[str(rand_num)]=make_argv

    return builds

def make_project(project_dir, builds, markdown_file=None):

    num_built = 0
    print "="*80
    print "MAKING PROJECT: ",project_dir
    os.chdir(project_dir)
    log = open("build.log", 'wt')
    print "-"*80

    FNULL = open(os.devnull,'w')

    results = {}

    for build in builds.keys():
        results[build]=-1

    print "Making: ",'Baseline',
    ret = subprocess.call(builds['Baseline'],stdout=log, stderr=subprocess.STDOUT)
    results['Baseline'] = ret
    if ret != 0:
        print ": Failed"
    else:
        print ": Success"
        num_built += 1
        for key in sorted(builds.keys()):
            args = builds[key]
            if key !='Baseline':
                print "Making: ",key,
                ret = subprocess.call(args,stdout=log, stderr=subprocess.STDOUT)
                results[key] = ret
                if ret == 0:
                    print ": Success"
                    num_built += 1
                else:
                    print ": Failed"
    if markdown_file:
        markdown_file.write('| %s | '%project_dir)
        for key in sorted(results.keys()):
            if results[key] == 0:
                markdown_file.write(' :white_check_mark: |')
            else:
                markdown_file.write(' :x: |')
        markdown_file.write(" \n")



    print "Built %i of %i binaries for %s"%(num_built,len(builds.keys()),project_dir)
    print "="*80
    os.chdir('..')
    return num_built

def get_padding(size_info):
    RAM_SIZES= [1,2,4,8,12,16,20,24,32,40,48,64,80,
                96,104,128,152,160,192,200,256]
    RAM_SIZES[:]=[x*1024 for x in RAM_SIZES]
    FLASH_SIZES = [2,4,8,16,32,64,128,256,512,544,768,1024]
    FLASH_SIZES[:]=[x*1024 for x in FLASH_SIZES]
    total_ram_usage = int(size_info[SIZE_DATA]) + int(size_info[SIZE_BSS]) + \
                      int(size_info[SIZE_STACK])*2 + int(size_info[SIZE_UNSAFE])*2
    for i in RAM_SIZES:
        if total_ram_usage< i:
            break;
    ram_slack = i -total_ram_usage

     #ram_slack/2 added because .data padding also takes space in flash
    total_flash_usage = int(size_info[SIZE_TEXT]) + ram_slack/2
    for i in FLASH_SIZES:
        if total_flash_usage< i:
            break;
    flash_slack = i - total_flash_usage

    return (ram_slack-40,flash_slack-40)  #-40 just for margin of error

def clean_project(project_dir):
    print "="*80
    print "CLEANING PROJECT: ",project_dir
    os.chdir(project_dir)
    ret = subprocess.call(['make','clean'])
    os.chdir('..')
    print "="*80



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-s','--setup',dest='setup', default=False,
                        action='store_true',
                        help= "Flag to indicate if setup should run")
    parser.add_argument('-m','--make',dest='make',default=False,
                        action='store_true',
                        help= "Flag to indicate if make should run on each project")
    parser.add_argument('-n','--num_rand',dest='num_rand',default=0,
                        type=int, help= "Number of Randomized binaries to create")
    parser.add_argument('-f','--funct_pad',dest='funct_pad',default=0,
                        type=int, help= "Number of padding functions to give")
    parser.add_argument('-d','--data_pad',dest='data_pad',default=0,
                        type=int, help= "Bytes to pad data with")
    parser.add_argument('-j','--json_size',dest='json_size_data',default=None,
                        help= "JSON file of binary info to build with")
    parser.add_argument('-c','--clean',dest='clean',default=False,
                        action='store_true',
                        help= "Flag to indicate if make clean should run")
    parser.add_argument('-o','--only',dest='build_only', default=None,
                        help='build only the specified benchmark')
    parser.add_argument('--markdown',dest='markdown', default=None,
                            help='Write build results to a markdown file')
    parser.add_argument('--no_regs',dest='no_regs', default=False,
                         action='store_true',
                        help='Exclude Register Randomization from builds')
    args=parser.parse_args()

    ret_val = 0
    num_projects = 0
    failed_projects = []
    passed_projects = {}
    total_built = 0

    if args.json_size_data:
        with open(args.json_size_data,'r') as infile:
            size_data_raw=json.load(infile)

        project_dirs = []
        size_data ={}
        for key in size_data_raw:
            if '--safestack--reduce_priv' in key:
                dir_name = key.split('--')[0]
                dir_name = SOURCE_ROOT + dir_name
                print dir_name
                project_dirs.append(dir_name)
                size_data[dir_name] = size_data_raw[key]
    else:
        project_dirs = os.listdir(SOURCE_ROOT)

    if args.build_only !=None:

        project_dirs =[os.path.join('..','src',args.build_only)]

    options={'no_regs':args.no_regs}
    if args.markdown:
        builds = gen_builds(args.num_rand,args.data_pad, args.funct_pad,options=options)
        markdown_file = open(args.markdown,'wt')
        markdown_file.write("# BEEBS Build Results\n\n")
        markdown_file.write("| Test | ")
        for key in sorted(builds.keys()):
            markdown_file.write("%s | "%key)
        markdown_file.write("\n")

        markdown_file.write("| :--- |")
        for i in range(len(builds.keys())):
            markdown_file.write(" :---: |")
        markdown_file.write("\n")
    else:
        markdown_file = None

    for project_name in sorted(project_dirs):
        project_dir=os.path.join(SOURCE_ROOT,project_name)
        makefile_am = os.path.join(project_dir,"Makefile.am")

        if os.path.isdir(project_dir) and os.path.isfile(makefile_am) \
           and not (project_dir in MAKE_EXCLUDE):
            num_projects += 1
            if args.setup:
                setup_project(project_dir)
            if args.clean:
                clean_project(project_dir)
            if args.make:
                if args.json_size_data:
                    (data_pad,funct_pad)=get_padding(size_data[project_name])
                else:
                    data_pad = args.data_pad
                    funct_pad = args.funct_pad

                builds = gen_builds(args.num_rand,data_pad,funct_pad,options=options)
                num_built = make_project(project_dir, builds, markdown_file)
                total_built += num_built
                if num_built != len(builds.keys()):
                    ret_val += 1
                    failed_projects.append(project_dir)
                else:
                    passed_projects[project_dir] = num_built
    if args.make:
        print "="*80
        print "Failed Projects"
        print "-"*80
        for failed_project in sorted(failed_projects):
            print "Failed: ",failed_project
        print "="*80
        print "Built %i Projects of %i"%(num_projects-len(failed_projects),num_projects)
        print "Built %i Binaries of %i"%(total_built,num_projects*(args.num_rand+2))
    if markdown_file:
        markdown_file.close()
    sys.exit(ret_val)

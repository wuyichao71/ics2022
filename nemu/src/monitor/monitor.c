/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/paddr.h>

// wuyc
#ifdef CONFIG_FTRACE
char *strtab = NULL;
Func_Hdr *func_hdr = NULL;
word_t func_num = 0;
#endif

void init_rand();
void init_log(const char *log_file);
void init_mem();
void init_difftest(char *ref_so_file, long img_size, int port);
void init_device();
void init_sdb();
void init_disasm(const char *triple);
/* wuyc */
/* void check_expr(); */
/* wuyc */

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
  /* Log("Exercise: Please remove me in the source code and compile NEMU again."); */
  assert(1);
}

#ifndef CONFIG_TARGET_AM
#include <getopt.h>
/* wuyc */
#ifdef CONFIG_FTRACE
#include <elf.h>
#endif
/* wuyc */

void sdb_set_batch_mode();

static char *log_file = NULL;
/* wuyc */
/* #ifdef CONFIG_FTRACE */
/* static char *elf_file = NULL; */
static ELF_NODE elf_header;
static ELF_NODE *elf_header_p = &elf_header;
static ELF_NODE *elf_p = &elf_header;
/* #endif */
/* wuyc */
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int difftest_port = 1234;

static long load_img() {
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    return 4096; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  Log("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(RESET_VECTOR), size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

/* wuyc */
#ifdef CONFIG_FTRACE
static void init_elf() {
  /* FILE *elfp = fopen(elf_file, "r"); */
  FILE *elfp = fopen(elf_header_p->next->elf_file, "r");
  if (elfp == NULL)
  {
    return;
  }

  Elf32_Ehdr ehdr;
  int ret = fread(&ehdr, sizeof(ehdr), 1, elfp);
	assert(ret == 1);
  /* printf("e_shoff = %d\n", ehdr.e_shoff); */
  /* printf("e_shnum = %d\n", ehdr.e_shnum); */
  /* printf("e_shstrndx = %d\n", ehdr.e_shstrndx); */
  fseek(elfp, ehdr.e_shoff, SEEK_SET);
  
  Elf32_Shdr *shdr = (Elf32_Shdr *)malloc(sizeof(Elf32_Shdr) * ehdr.e_shnum);

  /* read section header */
  for(int i = 0; i < ehdr.e_shnum; i++)
  {
    ret = fread(&shdr[i], sizeof(Elf32_Shdr), 1, elfp);
		assert(ret == 1);
  }

  /* read section header string table */
  Elf32_Shdr shstrtab_hdr = shdr[ehdr.e_shstrndx];
  char *shstrtab = (char *)malloc(shstrtab_hdr.sh_size*sizeof(char));
  fseek(elfp, shstrtab_hdr.sh_offset, SEEK_SET);
  ret = fread(shstrtab, shstrtab_hdr.sh_size, 1, elfp);
	assert(ret == 1);

  word_t symtab_ndx = 0, strtab_ndx = 0;
  for(int i = 0; i < ehdr.e_shnum; i++)
  {
    if(strcmp(shstrtab + shdr[i].sh_name, ".symtab") == 0)
      symtab_ndx = i;
    else if(strcmp(shstrtab + shdr[i].sh_name, ".strtab") == 0)
      strtab_ndx = i;
    /* printf("%s\n", shstrtab + shdr[i].sh_name); */
  }
  /* printf("symtab = %d, strtab = %d\n", symtab_ndx, strtab_ndx); */

  strtab = (char *)malloc(shdr[strtab_ndx].sh_size * sizeof(char));
  fseek(elfp, shdr[strtab_ndx].sh_offset, SEEK_SET);
  ret = fread(strtab, shdr[strtab_ndx].sh_size, 1, elfp);
	assert(ret == 1);
  /* printf("%s\n", strtab+1); */

  Elf32_Sym *symtab_hdr = (Elf32_Sym *)malloc(shdr[symtab_ndx].sh_size);
  fseek(elfp, shdr[symtab_ndx].sh_offset, SEEK_SET);
  ret = fread(symtab_hdr, shdr[symtab_ndx].sh_size, 1, elfp);
	assert(ret == 1);

  word_t symtab_num = shdr[symtab_ndx].sh_size / sizeof(Elf32_Sym);
  func_num = 0;
  for(int i = 0; i < symtab_num; i++)
  {
    if (ELF32_ST_TYPE(symtab_hdr[i].st_info) == STT_FUNC)
    {
      func_num++;
    }
  }

  func_hdr = (Func_Hdr *)malloc(func_num * sizeof(Func_Hdr));
  word_t fhdr_i = 0;
  for(int i = 0; i < symtab_num; i++)
  {
    if (ELF32_ST_TYPE(symtab_hdr[i].st_info) == STT_FUNC)
    {
      func_hdr[fhdr_i].st_name = symtab_hdr[i].st_name;
      func_hdr[fhdr_i].st_value = symtab_hdr[i].st_value;
      func_hdr[fhdr_i].st_size = symtab_hdr[i].st_size;
      /* printf("value = 0x%08x, size = %4d, name = %s\n", func_hdr[fhdr_i].st_value, func_hdr[fhdr_i].st_size, strtab + func_hdr[fhdr_i].st_name); */
      fhdr_i++;
    }
  }

  free(shdr);
  free(shstrtab);
  free(symtab_hdr);
  fclose(elfp);
}

void free_elf()
{
  free(strtab);
  free(func_hdr);
}
#endif
/* wuyc */

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    /* wuyc */
/* #ifdef CONFIG_FTRACE */
    {"elf"      , required_argument, NULL, 'e'},
/* #endif */
    /* wuyc */
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  /* wuyc */
  /* while ( (o = getopt_long(argc, argv, "-bhl:d:p:", table, NULL)) != -1) { */
  /* wuyc */
  while ( (o = getopt_long(argc, argv, "-bhl:d:p:e:", table, NULL)) != -1) {
    switch (o) {
      case 'b': sdb_set_batch_mode(); break;
      case 'p': sscanf(optarg, "%d", &difftest_port); break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      /* wuyc */
/* #ifdef CONFIG_FTRACE */
      case 'e': 
                elf_p->next = (ELF_NODE *)malloc(sizeof(ELF_NODE));
                elf_p = elf_p->next;
                elf_p->elf_file = optarg;
                elf_p->next = NULL;
                /* elf_file = optarg; */ 
                break;
/* #endif */
      /* wuyc */
      case 1: img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\t-e,--elf=FILE           input elf FILE\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

void init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Set random seed. */
  init_rand();

  /* Open the log file. */
  init_log(log_file);

  /* Initialize memory. */
  init_mem();

  /* Initialize devices. */
  IFDEF(CONFIG_DEVICE, init_device());

  /* wuyc */
#ifdef CONFIG_FTRACE
  init_elf();
#endif
  /* wuyc */

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Load the image to memory. This will overwrite the built-in image. */
  long img_size = load_img();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size, difftest_port);

  /* Initialize the simple debugger. */
  init_sdb();

#ifndef CONFIG_ISA_loongarch32r
  IFDEF(CONFIG_ITRACE, init_disasm(
    MUXDEF(CONFIG_ISA_x86,     "i686",
    MUXDEF(CONFIG_ISA_mips32,  "mipsel",
    MUXDEF(CONFIG_ISA_riscv32, "riscv32",
    MUXDEF(CONFIG_ISA_riscv64, "riscv64", "bad")))) "-pc-linux-gnu"
  ));
#endif

  /* Display welcome message. */
  /* wuyc */
  /* check_expr(); */
  /* wuyc */
  welcome();
}
#else // CONFIG_TARGET_AM
static long load_img() {
  extern char bin_start, bin_end;
  size_t size = &bin_end - &bin_start;
  Log("img size = %ld", size);
  memcpy(guest_to_host(RESET_VECTOR), &bin_start, size);
  return size;
}

void am_init_monitor() {
  init_rand();
  init_mem();
  init_isa();
  load_img();
  IFDEF(CONFIG_DEVICE, init_device());
  welcome();
}
#endif

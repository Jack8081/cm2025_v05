 OUTPUT_FORMAT("elf32-littlearm")
_region_min_align = 4;
MEMORY
{
 FLASH (rx) : ORIGIN = (0x10000000 + 0x0000), LENGTH = 0x100000
 SRAM (wx) : ORIGIN = 0x1000400, LENGTH = (120 * 1K - 0x400)
 PSRAM (rx) : ORIGIN = 0x18000000, LENGTH = (0 * 1K)
 SHARE_DSP_CONFIG_RAM (rw) : ORIGIN = 0x1082000, LENGTH = 0x600
 SHARE_DSP_BT_RAM (rw) : ORIGIN = 0x1082600, LENGTH = 0x100
 ANC_RAM (rw) : ORIGIN = 0x1082700, LENGTH = 0x0
 SHARE_RAM (rw) : ORIGIN = 0x1082700, LENGTH = 0x5900
 SRAM_EXT (rw) : ORIGIN = 0x1074000, LENGTH = 0x4000
 IDT_LIST (wx) : ORIGIN = (0x1000400 + (120 * 1K - 0x400)), LENGTH = 2K
}
ENTRY("__start")
SECTIONS
{
 .rel.plt :
 {
 *(.rel.plt)
 PROVIDE_HIDDEN (__rel_iplt_start = .);
 *(.rel.iplt)
 PROVIDE_HIDDEN (__rel_iplt_end = .);
 }
 .rela.plt :
 {
 *(.rela.plt)
 PROVIDE_HIDDEN (__rela_iplt_start = .);
 *(.rela.iplt)
 PROVIDE_HIDDEN (__rela_iplt_end = .);
 }
 .rel.dyn :
 {
 *(.rel.*)
 }
 .rela.dyn :
 {
 *(.rela.*)
 }
 /DISCARD/ :
 {
  *(.plt)
 }
 /DISCARD/ :
 {
  *(.iplt)
 }

 __rom_region_start = (0x10000000 + 0x0000);
 rom_start :
 {
. = 0x0;
. = ALIGN(4);
. = ALIGN( 1 << ((((4 * 32) <= 4) ? 2 : (((4 * 32) <= 8) ? 3 : (((4 * 32) <= 16) ? 4 : (((4 * 32) <= 32) ? 5 : (((4 * 32) <= 64) ? 6 : (((4 * 32) <= 128) ? 7 : (((4 * 32) <= 256) ? 8 : (((4 * 32) <= 512) ? 9 : (((4 * 32) <= 1024) ? 10 : (((4 * 32) <= 2048) ? 11 : (((4 * 32) <= 4096) ? 12 : (((4 * 32) <= 8192) ? 13 : (((4 * 32) <= 16384) ? 14 : (((4 * 32) <= 32768) ? 15:(((4 * 32) <= 65536) ? 16 : (((4 * 32) <= 131072) ? 17 : (((4 * 32) <= 262144) ? 18:(((4 * 32) <= 524288) ? 19 : (((4 * 32) <= 1048576) ? 20 : (((4 * 32) <= 2097152) ? 21 : (((4 * 32) <= 4194304) ? 22 : (((4 * 32) <= 8388608) ? 23 : (((4 * 32) <= 16777216) ? 24 : (((4 * 32) <= 33554432) ? 25 : (((4 * 32) <= 67108864) ? 26 : (((4 * 32) <= 134217728) ? 27 : (((4 * 32) <= 268435456) ? 28 : (((4 * 32) <= 536870912) ? 29 : (((4 * 32) <= 1073741824) ? 30 : (((4 * 32) <= 2147483648) ? 31 : 32))))))))))))))))))))))))))))))) );
. = ALIGN( 1 << ((((4 * (16 + 64)) <= 4) ? 2 : (((4 * (16 + 64)) <= 8) ? 3 : (((4 * (16 + 64)) <= 16) ? 4 : (((4 * (16 + 64)) <= 32) ? 5 : (((4 * (16 + 64)) <= 64) ? 6 : (((4 * (16 + 64)) <= 128) ? 7 : (((4 * (16 + 64)) <= 256) ? 8 : (((4 * (16 + 64)) <= 512) ? 9 : (((4 * (16 + 64)) <= 1024) ? 10 : (((4 * (16 + 64)) <= 2048) ? 11 : (((4 * (16 + 64)) <= 4096) ? 12 : (((4 * (16 + 64)) <= 8192) ? 13 : (((4 * (16 + 64)) <= 16384) ? 14 : (((4 * (16 + 64)) <= 32768) ? 15:(((4 * (16 + 64)) <= 65536) ? 16 : (((4 * (16 + 64)) <= 131072) ? 17 : (((4 * (16 + 64)) <= 262144) ? 18:(((4 * (16 + 64)) <= 524288) ? 19 : (((4 * (16 + 64)) <= 1048576) ? 20 : (((4 * (16 + 64)) <= 2097152) ? 21 : (((4 * (16 + 64)) <= 4194304) ? 22 : (((4 * (16 + 64)) <= 8388608) ? 23 : (((4 * (16 + 64)) <= 16777216) ? 24 : (((4 * (16 + 64)) <= 33554432) ? 25 : (((4 * (16 + 64)) <= 67108864) ? 26 : (((4 * (16 + 64)) <= 134217728) ? 27 : (((4 * (16 + 64)) <= 268435456) ? 28 : (((4 * (16 + 64)) <= 536870912) ? 29 : (((4 * (16 + 64)) <= 1073741824) ? 30 : (((4 * (16 + 64)) <= 2147483648) ? 31 : 32))))))))))))))))))))))))))))))) );
_vector_start = .;
KEEP(*(.exc_vector_table))
KEEP(*(".exc_vector_table.*"))
KEEP(*(.gnu.linkonce.irq_vector_table*))
KEEP(*(.vectors))
_vector_end = .;
 } > FLASH
 _ACTIONS_RODATA_SECTION_NAME :
 {
  . = ALIGN(4);
  __app_entry_table = .;
  KEEP(*(.app_entry))
  __app_entry_end = .;
  . = ALIGN(4);
  __service_entry_table = .;
  KEEP(*(.service_entry))
  __service_entry_end = .;
  . = ALIGN(4);
  __view_entry_table = .;
  KEEP(*(.view_entry))
  __view_entry_end = .;
 } > FLASH
 text :
 {
 __text_region_start = .;
  *(.text)
  *(".text.*")
  *(.gnu.linkonce.t.*)
  *(.glue_7t) *(.glue_7) *(.vfp11_veneer) *(.v4_bx)
 } > FLASH
 __text_region_end = .;
 .ARM.extab :
 {
  *(.ARM.extab* .gnu.linkonce.armextab.*)
 } > FLASH
 .ARM.exidx :
 {
  __start_unwind_idx = .;
  __exidx_start = .;
  *(.ARM.exidx* gnu.linkonce.armexidx.*)
  __exidx_end = .;
  __stop_unwind_idx = .;
 } > FLASH
 __rodata_region_start = .;
 initlevel :
 {
  __init_start = .;
  __init_PRE_KERNEL_1_start = .; KEEP(*(SORT(.z_init_PRE_KERNEL_1[0-9]_*))); KEEP(*(SORT(.z_init_PRE_KERNEL_1[1-9][0-9]_*)));
  __init_PRE_KERNEL_2_start = .; KEEP(*(SORT(.z_init_PRE_KERNEL_2[0-9]_*))); KEEP(*(SORT(.z_init_PRE_KERNEL_2[1-9][0-9]_*)));
  __init_POST_KERNEL_start = .; KEEP(*(SORT(.z_init_POST_KERNEL[0-9]_*))); KEEP(*(SORT(.z_init_POST_KERNEL[1-9][0-9]_*)));
  __init_APPLICATION_start = .; KEEP(*(SORT(.z_init_APPLICATION[0-9]_*))); KEEP(*(SORT(.z_init_APPLICATION[1-9][0-9]_*)));
  __init_SMP_start = .; KEEP(*(SORT(.z_init_SMP[0-9]_*))); KEEP(*(SORT(.z_init_SMP[1-9][0-9]_*)));
  __init_end = .;
 } > FLASH
 devices :
 {
  __device_start = .;
  __device_PRE_KERNEL_1_start = .; KEEP(*(SORT(.z_device_PRE_KERNEL_1[0-9]_*))); KEEP(*(SORT(.z_device_PRE_KERNEL_1[1-9][0-9]_*)));
  __device_PRE_KERNEL_2_start = .; KEEP(*(SORT(.z_device_PRE_KERNEL_2[0-9]_*))); KEEP(*(SORT(.z_device_PRE_KERNEL_2[1-9][0-9]_*)));
  __device_POST_KERNEL_start = .; KEEP(*(SORT(.z_device_POST_KERNEL[0-9]_*))); KEEP(*(SORT(.z_device_POST_KERNEL[1-9][0-9]_*)));
  __device_APPLICATION_start = .; KEEP(*(SORT(.z_device_APPLICATION[0-9]_*))); KEEP(*(SORT(.z_device_APPLICATION[1-9][0-9]_*)));
  __device_SMP_start = .; KEEP(*(SORT(.z_device_SMP[0-9]_*))); KEEP(*(SORT(.z_device_SMP[1-9][0-9]_*)));
  __device_end = .;
 } > FLASH
 initlevel_error :
 {
  KEEP(*(SORT(.z_init_[_A-Z0-9]*)))
 }
 ASSERT(SIZEOF(initlevel_error) == 0, "Undefined initialization levels used.")
 app_shmem_regions : ALIGN_WITH_INPUT
 {
  __app_shmem_regions_start = .;
  KEEP(*(SORT(.app_regions.*)));
  __app_shmem_regions_end = .;
 } > FLASH
 bt_l2cap_fixed_chan_area : SUBALIGN(4) { _bt_l2cap_fixed_chan_list_start = .; KEEP(*(SORT_BY_NAME(._bt_l2cap_fixed_chan.static.*))); _bt_l2cap_fixed_chan_list_end = .; } > FLASH
 bt_l2cap_br_fixed_chan_area : SUBALIGN(4) { _bt_l2cap_br_fixed_chan_list_start = .; KEEP(*(SORT_BY_NAME(._bt_l2cap_br_fixed_chan.static.*))); _bt_l2cap_br_fixed_chan_list_end = .; } > FLASH
 bt_conn_cb_area : SUBALIGN(4) { _bt_conn_cb_list_start = .; KEEP(*(SORT_BY_NAME(._bt_conn_cb.static.*))); _bt_conn_cb_list_end = .; } > FLASH
 bt_gatt_service_static_area : SUBALIGN(4) { _bt_gatt_service_static_list_start = .; KEEP(*(SORT_BY_NAME(._bt_gatt_service_static.static.*))); _bt_gatt_service_static_list_end = .; } > FLASH
 k_p4wq_initparam_area : SUBALIGN(4) { _k_p4wq_initparam_list_start = .; KEEP(*(SORT_BY_NAME(._k_p4wq_initparam.static.*))); _k_p4wq_initparam_list_end = .; } > FLASH
 log_strings_sections : ALIGN_WITH_INPUT
 {
  __log_strings_start = .;
  KEEP(*(SORT(.log_strings*)));
  __log_strings_end = .;
 } > FLASH
 log_const_sections : ALIGN_WITH_INPUT
 {
  __log_const_start = .;
  KEEP(*(SORT(.log_const_*)));
  __log_const_end = .;
 } > FLASH
 log_backends_sections : ALIGN_WITH_INPUT
 {
  __log_backends_start = .;
  KEEP(*("._log_backend.*"));
  __log_backends_end = .;
 } > FLASH
 shell_area : SUBALIGN(4) { _shell_list_start = .; KEEP(*(SORT_BY_NAME(._shell.static.*))); _shell_list_end = .; } > FLASH
 shell_root_cmds_sections : ALIGN_WITH_INPUT
 {
  __shell_root_cmds_start = .;
  KEEP(*(SORT(.shell_root_cmd_*)));
  __shell_root_cmds_end = .;
 } > FLASH
 font_entry_sections : ALIGN_WITH_INPUT
 {
  __font_entry_start = .;
  KEEP(*(SORT_BY_NAME("._cfb_font.*")))
  __font_entry_end = .;
 } > FLASH
 tracing_backend_area : SUBALIGN(4) { _tracing_backend_list_start = .; KEEP(*(SORT_BY_NAME(._tracing_backend.static.*))); _tracing_backend_list_end = .; } > FLASH
 zephyr_dbg_info : ALIGN_WITH_INPUT
 {
  KEEP(*(".dbg_thread_info"));
 } > FLASH
 device_handles : ALIGN_WITH_INPUT
 {
  __device_handles_start = .;
  KEEP(*(SORT(.__device_handles_pass2*)));
  __device_handles_end = .;
 } > FLASH
    rodata :
 {
  *(.rodata)
  *(".rodata.*")
  *(.gnu.linkonce.r.*)
     . = ALIGN(4);
  __overlay_table = .;
  LONG(0x4c564f53)
  LONG(7)
  LONG(0x70766177)
  LONG(0);
  LONG(0);
  LONG(0);
  LONG(ABSOLUTE(ADDR(.overlay.data.apwav)));
  LONG(SIZEOF(.overlay.data.apwav));
  LONG(LOADADDR(.overlay.data.apwav));
  LONG(ABSOLUTE(ADDR(.overlay.bss.apwav)));
  LONG(SIZEOF(.overlay.bss.apwav));
  LONG(0x7033706d)
  LONG(0);
  LONG(0);
  LONG(0);
  LONG(ABSOLUTE(ADDR(.overlay.data.apmp3)));
  LONG(SIZEOF(.overlay.data.apmp3));
  LONG(LOADADDR(.overlay.data.apmp3));
  LONG(ABSOLUTE(ADDR(.overlay.bss.apmp3)));
  LONG(SIZEOF(.overlay.bss.apmp3));
  LONG(0x70657061)
  LONG(0);
  LONG(0);
  LONG(0);
  LONG(ABSOLUTE(ADDR(.overlay.data.apape)));
  LONG(SIZEOF(.overlay.data.apape));
  LONG(LOADADDR(.overlay.data.apape));
  LONG(ABSOLUTE(ADDR(.overlay.bss.apape)));
  LONG(SIZEOF(.overlay.bss.apape));
  LONG(0x70616d77)
  LONG(0);
  LONG(0);
  LONG(0);
  LONG(ABSOLUTE(ADDR(.overlay.data.apwma)));
  LONG(SIZEOF(.overlay.data.apwma));
  LONG(LOADADDR(.overlay.data.apwma));
  LONG(ABSOLUTE(ADDR(.overlay.bss.apwma)));
  LONG(SIZEOF(.overlay.bss.apwma));
  LONG(0x64616c66)
  LONG(0);
  LONG(0);
  LONG(0);
  LONG(ABSOLUTE(ADDR(.overlay.data.apfla)));
  LONG(SIZEOF(.overlay.data.apfla));
  LONG(LOADADDR(.overlay.data.apfla));
  LONG(ABSOLUTE(ADDR(.overlay.bss.apfla)));
  LONG(SIZEOF(.overlay.bss.apfla));
  LONG(0x64636177)
  LONG(0);
  LONG(0);
  LONG(0);
  LONG(ABSOLUTE(ADDR(.overlay.data.apaac)));
  LONG(SIZEOF(.overlay.data.apaac));
  LONG(LOADADDR(.overlay.data.apaac));
  LONG(ABSOLUTE(ADDR(.overlay.bss.apaac)));
  LONG(SIZEOF(.overlay.bss.apaac));
  . = ALIGN(4);
  . = ALIGN(4);
 } > FLASH
 __rodata_region_end = .;
 . = ALIGN(_region_min_align);
 __rom_region_end = .;
 __rom_region_size = __rom_region_end - __rom_region_start;
   
 /DISCARD/ : {
  *(.got.plt)
  *(.igot.plt)
  *(.got)
  *(.igot)
 }
   
 . = 0x1000400;
 . = ALIGN(_region_min_align);
 _image_ram_start = .;
 _SRAM_BSS_SECTION_NAME (NOLOAD) : ALIGN_WITH_INPUT
 {
  . = ALIGN(4);
  __kernel_ram_start = .;
  *(.dsp_hex_buf*)
  *(.media_mem_pool*)
  *(.interrupt.noinit.stack*)
  *(.page_mempool*)
  . = ALIGN(4096);
  *(.srm_irq_vector*)
  *(.codec.noinit.stack*)
  *(.main.noinit.stack*)
  *(.system.bss.sdfs_cache*)
  *(.diskio.cache.pool*)
  *(.ram.noinit*)
  __kernel_ram_save_end = .;
 } > SRAM AT > SRAM
 OVERLAY : NOCROSSREFS
 {
  .overlay.data.apfla {
   *a1_fla_p.a:*(.data .data.*)
  }
  .overlay.data.apwma {
   *a1_w13_p.a:*(.data .data.*)
  }
  .overlay.data.apape {
   *a1_ape_p.a:*(.data .data.*)
  }
  .overlay.data.apmp3 {
   *a1_mp3_p.a:*(.data .data.*)
  }
  .overlay.data.apwav {
   *a1_wav_p.a:*(.data .data.*)
  }
  .overlay.data.apaac {
   *a1_a13_p.a:*(.data .data.*)
  }
 } > SRAM AT > FLASH
.ramfunc : ALIGN_WITH_INPUT
{
 . = ALIGN(_region_min_align);
 __ramfunc_start = .;
 KEEP(*(.rom_vector))
 *(.ramfunc)
 *(".ramfunc.*")
 . = ALIGN(_region_min_align);
 __ramfunc_end = .;
} > SRAM AT > FLASH
__ramfunc_size = __ramfunc_end - __ramfunc_start;
__ramfunc_load_start = LOADADDR(.ramfunc);
 datas : ALIGN_WITH_INPUT
 {
  __data_region_start = .;
  __data_start = .;
  *(.data)
  *(".data.*")
  *(".kernel.*")
 __data_end = .;
 } > SRAM AT > FLASH
    __data_size = __data_end - __data_start;
    __data_load_start = LOADADDR(datas);
    __data_region_load_start = LOADADDR(datas);
 sw_isr_table : ALIGN_WITH_INPUT
 {
  . = ALIGN(0);
  *(.gnu.linkonce.sw_isr_table*)
 } > SRAM AT > FLASH
        device_states : ALIGN_WITH_INPUT
        {
                __device_states_start = .;
  KEEP(*(".z_devstate"));
  KEEP(*(".z_devstate.*"));
                __device_states_end = .;
        } > SRAM AT > FLASH
 pm_device_slots (NOLOAD) : ALIGN_WITH_INPUT
 {
  __pm_device_slots_start = .;
  KEEP(*(".z_pm_device_slots"));
  __pm_device_slots_end = .;
 } > SRAM AT > FLASH
 initshell : ALIGN_WITH_INPUT
 {
  __shell_module_start = .;
  KEEP(*(".shell_module_*"));
  __shell_module_end = .;
  __shell_cmd_start = .;
  KEEP(*(".shell_cmd_*"));
  __shell_cmd_end = .;
 } > SRAM AT > FLASH
 log_dynamic_sections : ALIGN_WITH_INPUT
 {
  __log_dynamic_start = .;
  KEEP(*(SORT(.log_dynamic_*)));
  __log_dynamic_end = .;
 } > SRAM AT > FLASH
 _static_thread_data_area : ALIGN_WITH_INPUT SUBALIGN(4) { __static_thread_data_list_start = .; KEEP(*(SORT_BY_NAME(.__static_thread_data.static.*))); __static_thread_data_list_end = .; } > SRAM AT > FLASH
 k_timer_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_timer_list_start = .; *(SORT_BY_NAME(._k_timer.static.*)); _k_timer_list_end = .; } > SRAM AT > FLASH
 k_mem_slab_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_mem_slab_list_start = .; *(SORT_BY_NAME(._k_mem_slab.static.*)); _k_mem_slab_list_end = .; } > SRAM AT > FLASH
 k_mem_pool_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_mem_pool_list_start = .; *(SORT_BY_NAME(._k_mem_pool.static.*)); _k_mem_pool_list_end = .; } > SRAM AT > FLASH
 k_heap_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_heap_list_start = .; *(SORT_BY_NAME(._k_heap.static.*)); _k_heap_list_end = .; } > SRAM AT > FLASH
 k_mutex_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_mutex_list_start = .; *(SORT_BY_NAME(._k_mutex.static.*)); _k_mutex_list_end = .; } > SRAM AT > FLASH
 k_stack_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_stack_list_start = .; *(SORT_BY_NAME(._k_stack.static.*)); _k_stack_list_end = .; } > SRAM AT > FLASH
 k_msgq_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_msgq_list_start = .; *(SORT_BY_NAME(._k_msgq.static.*)); _k_msgq_list_end = .; } > SRAM AT > FLASH
 k_mbox_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_mbox_list_start = .; *(SORT_BY_NAME(._k_mbox.static.*)); _k_mbox_list_end = .; } > SRAM AT > FLASH
 k_pipe_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_pipe_list_start = .; *(SORT_BY_NAME(._k_pipe.static.*)); _k_pipe_list_end = .; } > SRAM AT > FLASH
 k_sem_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_sem_list_start = .; *(SORT_BY_NAME(._k_sem.static.*)); _k_sem_list_end = .; } > SRAM AT > FLASH
 k_queue_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_queue_list_start = .; *(SORT_BY_NAME(._k_queue.static.*)); _k_queue_list_end = .; } > SRAM AT > FLASH
 k_condvar_area : ALIGN_WITH_INPUT SUBALIGN(4) { _k_condvar_list_start = .; *(SORT_BY_NAME(._k_condvar.static.*)); _k_condvar_list_end = .; } > SRAM AT > FLASH
 _net_buf_pool_area : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _net_buf_pool_list = .;
  KEEP(*(SORT_BY_NAME("._net_buf_pool.static.*")))
  _net_buf_pool_list_end = .;
 } > SRAM AT > FLASH
 usb_descriptor : ALIGN_WITH_INPUT SUBALIGN(1)
 {
  __usb_descriptor_start = .;
  *(".usb.descriptor")
  KEEP(*(SORT_BY_NAME(".usb.descriptor*")))
  __usb_descriptor_end = .;
 } > SRAM AT > FLASH
 usb_data : ALIGN_WITH_INPUT SUBALIGN(1)
 {
  __usb_data_start = .;
  *(".usb.data")
  KEEP(*(SORT_BY_NAME(".usb.data*")))
  __usb_data_end = .;
 } > SRAM AT > FLASH
    __data_region_end = .;
 bss (NOLOAD) : ALIGN_WITH_INPUT
 {
  . = ALIGN(4);
  __bss_start = .;
  *(EXCLUDE_FILE (*libdecencsbc.a:* *libdecaac.a:* *libdecape.a:* *libdecflac.a:* *libdecamr.a:* *libdecwav.a:* *libdecmp3.a:* *libdecwma.a:* *a1_act_d.a:* *libencamr.a:* *libwav.a:* *a1_wav_p.a:* *a1_w13_p.a:* *a1_ape_p.a:* *a1_mp3_p.a:* *a1_fla_p.a:* *a1_a13_p.a:* *libkshow.a:* *libencpp.a:* *libencopus.a:* *libencspeex.a:* *libdnoise.a:* *libCSpotter_16k24d.a:* *libresample.a:*) .bss)
  *(EXCLUDE_FILE (*libdecencsbc.a:* *libdecaac.a:* *libdecape.a:* *libdecflac.a:* *libdecamr.a:* *libdecwav.a:* *libdecmp3.a:* *libdecwma.a:* *a1_act_d.a:* *libencamr.a:* *libwav.a:* *a1_wav_p.a:* *a1_w13_p.a:* *a1_ape_p.a:* *a1_mp3_p.a:* *a1_fla_p.a:* *a1_a13_p.a:* *libkshow.a:* *libencpp.a:* *libencopus.a:* *libencspeex.a:* *libdnoise.a:* *libCSpotter_16k24d.a:* *libresample.a:*) .bss.*)
  *(EXCLUDE_FILE (*libdecencsbc.a:* *libdecaac.a:* *libdecape.a:* *libdecflac.a:* *libdecamr.a:* *libdecwav.a:* *libdecmp3.a:* *libdecwma.a:* *a1_act_d.a:* *libencamr.a:* *libwav.a:* *a1_wav_p.a:* *a1_w13_p.a:* *a1_ape_p.a:* *a1_mp3_p.a:* *a1_fla_p.a:* *a1_a13_p.a:* *libkshow.a:* *libencpp.a:* *libencopus.a:* *libencspeex.a:* *libdnoise.a:* *libCSpotter_16k24d.a:* *libresample.a:*) .scommon)
  *(EXCLUDE_FILE (*libdecencsbc.a:* *libdecaac.a:* *libdecape.a:* *libdecflac.a:* *libdecamr.a:* *libdecwav.a:* *libdecmp3.a:* *libdecwma.a:* *a1_act_d.a:* *libencamr.a:* *libwav.a:* *a1_wav_p.a:* *a1_w13_p.a:* *a1_ape_p.a:* *a1_mp3_p.a:* *a1_fla_p.a:* *a1_a13_p.a:* *libkshow.a:* *libencpp.a:* *libencopus.a:* *libencspeex.a:* *libdnoise.a:* *libCSpotter_16k24d.a:* *libresample.a:*) COMMON)
  *(".kernel_bss.*")
  *(.bthost_bss*)
  *(.btsrv_bss*)
  __bss_end = ALIGN(4);
 } > SRAM AT > SRAM
 _SLEEP_NOT_SAVE_SECTION_NAME (NOLOAD) :
 {
            __sleepnosave_ram_start = .;
  *(.act_s2_not_save_mem*)
            __sleepnosave_ram_end = .;
 }> SRAM
 OVERLAY : NOCROSSREFS
 {
  .overlay.bss.apfla {
   *a1_fla_p.a:*(.bss .bss.* .scommon COMMON)
  }
  .overlay.bss.apwma {
   *a1_w13_p.a:*(.bss .bss.* .scommon COMMON)
  }
  .overlay.bss.apape {
   *a1_ape_p.a:*(.bss .bss.* .scommon COMMON)
  }
  .overlay.bss.apmp3 {
   *a1_mp3_p.a:*(.bss .bss.* .scommon COMMON)
  }
  .overlay.bss.apwav {
   *a1_wav_p.a:*(.bss .bss.* .scommon COMMON)
  }
  .overlay.bss.apaac {
   *a1_a13_p.a:*(.bss .bss.* .scommon COMMON)
  }
 } > SRAM
 noinit (NOLOAD) :
 {
  *(.noinit)
  *(".noinit.*")
  *(".kernel_noinit.*")
 } > SRAM
 _image_ram_end = .;
 _end = .;
 __kernel_ram_end = 0x1000400 + (120 * 1K - 0x400);
 __kernel_ram_size = __kernel_ram_end - __kernel_ram_start;

 _SHARE_DSP_CONFIG_RAM_BSS_SECTION_NAME (NOLOAD) : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  __share_dsp_config_ram_start = .;
  *(.DSP_SHARE_CONFIG_RAM*)
 } > SHARE_DSP_CONFIG_RAM
 __share_dsp_config_ram_end = .;
 _SHARE_DSP_BT_RAM_BSS_SECTION_NAME (NOLOAD) : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  __share_dsp_bt_ram_start = .;
  *(.DSP_BT_SHARE_RAM*)
 } > SHARE_DSP_BT_RAM
 __share_dsp_bt_ram_end = .;
 _SHARE_RAM_BSS_SECTION_NAME (NOLOAD) : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  __share_ram_start = .;
  *(.DSP_SHARE_RAM*)
  __share_media_ram_start = .;
  *(.DSP_SHARE_MEDIA_RAM*)
  __share_media_ram_end = .;
 } > SHARE_RAM
 __share_ram_end = .;
 _ANC_RAM_BSS_SECTION_NAME (NOLOAD) : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  __anc_ram_start = .;
  *(.ANC_SHARE_RAM*)
 } > ANC_RAM
 __anc_ram_end = .;
 _EXT_RAM_BSS_SECTION_NAME (NOLOAD) : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  __ext_ram_start = .;
  *(.SRAM_EXT_RAM*)
 } > SRAM_EXT
 __ext_ram_end = .;
 _PRAM_BSS_SECTION_NAME (NOLOAD) : ALIGN_WITH_INPUT SUBALIGN(0x400)
 {
  *(.UI_PSRAM_REGION*)
 } > PSRAM
/DISCARD/ :
{
 KEEP(*(.irq_info*))
 KEEP(*(.intList*))
}
 .stab 0 : { *(.stab) }
 .stabstr 0 : { *(.stabstr) }
 .stab.excl 0 : { *(.stab.excl) }
 .stab.exclstr 0 : { *(.stab.exclstr) }
 .stab.index 0 : { *(.stab.index) }
 .stab.indexstr 0 : { *(.stab.indexstr) }
 .gnu.build.attributes 0 : { *(.gnu.build.attributes .gnu.build.attributes.*) }
 .comment 0 : { *(.comment) }
 .debug 0 : { *(.debug) }
 .line 0 : { *(.line) }
 .debug_srcinfo 0 : { *(.debug_srcinfo) }
 .debug_sfnames 0 : { *(.debug_sfnames) }
 .debug_aranges 0 : { *(.debug_aranges) }
 .debug_pubnames 0 : { *(.debug_pubnames) }
 .debug_info 0 : { *(.debug_info .gnu.linkonce.wi.*) }
 .debug_abbrev 0 : { *(.debug_abbrev) }
 .debug_line 0 : { *(.debug_line .debug_line.* .debug_line_end ) }
 .debug_frame 0 : { *(.debug_frame) }
 .debug_str 0 : { *(.debug_str) }
 .debug_loc 0 : { *(.debug_loc) }
 .debug_macinfo 0 : { *(.debug_macinfo) }
 .debug_weaknames 0 : { *(.debug_weaknames) }
 .debug_funcnames 0 : { *(.debug_funcnames) }
 .debug_typenames 0 : { *(.debug_typenames) }
 .debug_varnames 0 : { *(.debug_varnames) }
 .debug_pubtypes 0 : { *(.debug_pubtypes) }
 .debug_ranges 0 : { *(.debug_ranges) }
 .debug_macro 0 : { *(.debug_macro) }
    /DISCARD/ : { *(.note.GNU-stack) }
    .ARM.attributes 0 :
 {
  KEEP(*(.ARM.attributes))
  KEEP(*(.gnu.attributes))
 }
 .last_section (NOLOAD) :
 {
 } > FLASH
 _flash_used = LOADADDR(.last_section) - __rom_region_start;
}

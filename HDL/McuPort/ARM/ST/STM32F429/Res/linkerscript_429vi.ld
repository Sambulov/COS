ENTRY(reset_handler)

MEMORY
{
	FLASH (RX)   : ORIGIN = 0x08000000 + @BOOT_OFFSET@, LENGTH = 2M - @BOOT_OFFSET@
	SRAM (RWX)   : ORIGIN = 0x20000000, LENGTH = 192K
	CCMRAM (RWX) : ORIGIN = 0x10000000, LENGTH = 64K
}

_stack_size = @STACK_RESERV@;
_stack_end   = ORIGIN(SRAM) + LENGTH(SRAM);
_stack_start = _stack_end - _stack_size;
_estack = _stack_end;

SECTIONS
{
	.isr_vector (READONLY):
	{
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} > FLASH

	.text (READONLY):
	{
		. = ALIGN(4);
		_stext = .;

		*(.text)
		*(.text*)
		*(.rodata)
		*(.rodata*)
		*(.glue_7)
		*(.glue_7t)
		KEEP(*(.init))
		KEEP(*(.fini))
		. = ALIGN(4);
		_etext = .;

	} > FLASH

	.ARM.extab (READONLY):
	{
		. = ALIGN(4);
		*(.ARM.extab)
		*(.gnu.linkonce.armextab.*)
		. = ALIGN(4);
	} > FLASH

	.exidx (READONLY):
	{
		. = ALIGN(4);
		PROVIDE(__exidx_start = .);
		*(.ARM.exidx*)
		. = ALIGN(4);
		PROVIDE(__exidx_end = .);
	} > FLASH

	.ARM.attributes (READONLY):
	{
		*(.ARM.attributes)
	} > FLASH

	.preinit_array (READONLY):
	{
		PROVIDE(__preinit_array_start = .);
		KEEP(*(.preinit_array*))
		PROVIDE(__preinit_array_end = .);
	} > FLASH

	.init_array (READONLY):
	{
		PROVIDE(__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array*))
		PROVIDE(__init_array_end = .);
	} > FLASH

	.fini_array (READONLY):
	{
		PROVIDE(__fini_array_start = .);
		KEEP(*(.fini_array*))
		KEEP(*(SORT(.fini_array.*)))
		PROVIDE(__fini_array_end = .);
	} > FLASH

	. = ALIGN(4);
	_sidata = .;

	.data : AT(_sidata)
	{
		. = ALIGN(4);
		_sdata = .;

		PROVIDE(__data_start__ = _sdata);
		*(.data)
		*(.data*)
		. = ALIGN(4);
		_edata = .;

		PROVIDE(__data_end__ = _edata);
	} > SRAM

	.bss :
	{
		. = ALIGN(4);
		_sbss = .;

		PROVIDE(__bss_start__ = _sbss);
		*(.bss)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;

		PROVIDE(__bss_end__ = _ebss);
	} > SRAM

    ASSERT(_ebss <= _stack_start, "SRAM overflow: .bss and .data exceed stack start")

    PROVIDE(end = .);
    _free_ram_start = .;
    _free_ram_end   = _stack_start;
    _free_ram_size  = _free_ram_end - _free_ram_start;

	.ccm :
	{
		. = ALIGN(4);
		_sccm = .;
		PROVIDE(__ccm_start__ = _sccm);
		*(.ccm)
		*(.ccm*)
		. = ALIGN(4);
		_eccm = .;
		PROVIDE(__ccm_end__ = _eccm);
	} > CCMRAM
}

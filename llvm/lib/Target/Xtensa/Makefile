LEVEL = ../../..
LIBRARYNAME = LLVMXtensaCodeGen
TARGET = Xtensa

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = XtensaGenRegisterInfo.inc \
		XtensaGenAsmWriter.inc \
		XtensaGenAsmMatcher.inc \
		XtensaGenInstrInfo.inc \
		XtensaGenDAGISel.inc \
		XtensaGenSubtargetInfo.inc \
		XtensaGenCallingConv.inc \
		XtensaGenMCCodeEmitter.inc

DIRS = InstPrinter AsmParser TargetInfo MCTargetDesc

include $(LEVEL)/Makefile.common


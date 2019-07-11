add_llvm_library(LLVMXtensaDesc
  XtensaMCAsmBackend.cpp
  XtensaMCAsmInfo.cpp
  XtensaMCCodeEmitter.cpp
  XtensaMCExpr.cpp
  XtensaMCObjectWriter.cpp
  XtensaMCTargetDesc.cpp
  )

add_dependencies(LLVMXtensaDesc XtensaCommonTableGen)

#=============
# Memory Map
#=============
GDT             = 0x0000000000000000
IDT             = 0x0000000000000800
OSCode          = 0x0000000000001000
OSData          = 0x0000000000010000
StaticPort      = 0x000000000006F000
TaskStruct      = 0x0000000000080000
TempUserCode    = 0x0000000000095000
TempUserData    = 0x0000000000096000
TempUStack      = 0x0000000000098000
PageMap         = 0x0000000000100000
UserCode        = 0x0000000000400000
UserData        = 0x0000000000600000
KernelStack     = 0x0000000000800000
UserStack       = 0x0000000000a00000

PageSize    = 0x1000
KbdPort     = StaticPort + MP.size

OsCodeSeg   = 0x8
OsDataSeg   = 0x10
code64      = 0x18
data64      = 0x20
udata64     = 0x28
user64      = 0x30
tssd64      = 0x38
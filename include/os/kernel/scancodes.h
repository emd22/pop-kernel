#ifndef SCANCODES_H
#define SCANCODES_H

#define SCANCODE_ERROR 0x00
#define SCANCODE_ESC 0x01
#define SCANCODE_1 0x02
#define SCANCODE_2 0x03
#define SCANCODE_3 0x04
#define SCANCODE_4 0x05
#define SCANCODE_5 0x06
#define SCANCODE_6 0x07
#define SCANCODE_7 0x08
#define SCANCODE_8 0x09
#define SCANCODE_9 0x0A
#define SCANCODE_0 0x0B
#define SCANCODE_DASH 0x0C
#define SCANCODE_EQUAL 0x0D
#define SCANCODE_BACKSPACE 0x0E
#define SCANCODE_TAB 0x0F
#define SCANCODE_Q 0x10
#define SCANCODE_W 0x11
#define SCANCODE_E 0x12
#define SCANCODE_R 0x13
#define SCANCODE_T 0x14
#define SCANCODE_Y 0x15
#define SCANCODE_U 0x16
#define SCANCODE_I 0x17
#define SCANCODE_O 0x18
#define SCANCODE_P 0x19
#define SCANCODE_LEFT_BRACKET 0x1A
#define SCANCODE_RIGHT_BRACKET 0x1B
#define SCANCODE_RETURN 0x1C
#define SCANCODE_LEFT_CTRL 0x1D
#define SCANCODE_A 0x1E
#define SCANCODE_S 0x1F
#define SCANCODE_D 0x20
#define SCANCODE_F 0x21
#define SCANCODE_G 0x22
#define SCANCODE_H 0x23
#define SCANCODE_J 0x24
#define SCANCODE_K 0x25
#define SCANCODE_L 0x26
#define SCANCODE_SEMICOLON 0x27
#define SCANCODE_QUOTE 0x28
#define SCANCODE_BACK_QUOTE 0x29
#define SCANCODE_LEFT_SHIFT 0x2A
#define SCANCODE_BACKSLASH 0x2B
#define SCANCODE_Z 0x2C
#define SCANCODE_X 0x2D
#define SCANCODE_C 0x2E
#define SCANCODE_V 0x2F
#define SCANCODE_B 0x30
#define SCANCODE_N 0x31
#define SCANCODE_M 0x32
#define SCANCODE_COMMA 0x33
#define SCANCODE_PERIOD 0x34
#define SCANCODE_FORWARDSLASH 0x35
#define SCANCODE_RIGHT_SHIFT 0x36
#define SCANCODE_PRINTSCREEN 0x37
#define SCANCODE_LEFT_ALT 0x38
#define SCANCODE_SPACEBAR 0x39
#define SCANCODE_LEFT_ARROW 0x4B
#define SCANCODE_RIGHT_ARROW 0x4D
#define SCANCODE_UP_ARROW 0x48
#define SCANCODE_DOWN_ARROW 0x50

// enum KEYCODE {
//     NULL_KEY = 0,
//     //-----------------
//     K_1_PRESSED = 0x02,
//     K_1_RELEASED = 0x82,
//     K_2_PRESSED = 0x03,
//     K_2_RELEASED = 0x83,  
//     K_3_PRESSED = 0x04,
//     K_3_RELEASED = 0x84,
//     K_4_PRESSED = 0x05,
//     K_4_RELEASED = 0x85,
//     K_5_PRESSED = 0x06,
//     K_5_RELEASED = 0x86,
//     K_6_PRESSED = 0x07,
//     K_6_RELEASED = 0x87,
//     K_7_PRESSED = 0x08,
//     K_7_RELEASED = 0x88,
//     K_8_PRESSED = 0x09,
//     K_8_RELEASED = 0x89,
//     K_9_PRESSED = 0x0A,
//     K_9_RELEASED = 0x8A,
//     K_0_PRESSED = 0x0B,
//     K_0_RELEASED = 0x8B,
//     //------------------
//     Q_PRESSED = 0x10,
//     Q_RELEASED = 0x90,
//     W_PRESSED = 0x11,
//     W_RELEASED = 0x91,
//     E_PRESSED = 0x12,
//     E_RELEASED = 0x92,
//     R_PRESSED = 0x13,
//     R_RELEASED = 0x93,
//     T_PRESSED = 0x14,
//     T_RELEASED = 0x94,
//     Z_PRESSED = 0x15,
//     Z_RELEASED = 0x95,
//     U_PRESSED = 0x16,
//     U_RELEASED = 0x96,
//     I_PRESSED = 0x17,
//     I_RELEASED = 0x97,
//     O_PRESSED = 0x18,
//     O_RELEASED = 0x98,
//     P_PRESSED = 0x19,
//     P_RELEASED = 0x99,
//     A_PRESSED = 0x1E,
//     A_RELEASED = 0x9E,
//     S_PRESSED = 0x1F,
//     S_RELEASED = 0x9F,
//     D_PRESSED = 0x20,
//     D_RELEASED = 0xA0,
//     F_PRESSED = 0x21,
//     F_RELEASED = 0xA1,
//     G_PRESSED = 0x22,
//     G_RELEASED = 0xA2,
//     H_PRESSED = 0x23,
//     H_RELEASED = 0xA3,
//     J_PRESSED = 0x24,
//     J_RELEASED = 0xA4,
//     K_PRESSED = 0x25,
//     K_RELEASED = 0xA5,
//     L_PRESSED = 0x26,
//     L_RELEASED = 0xA6,
//     Y_PRESSED = 0x2C,
//     Y_RELEASED = 0xAC,
//     X_PRESSED = 0x2D,
//     X_RELEASED = 0xAD,
//     C_PRESSED = 0x2E,
//     C_RELEASED = 0xAE,
//     V_PRESSED = 0x2F,
//     V_RELEASED = 0xAF,
//     B_PRESSED = 0x30,
//     B_RELEASED = 0xB0,
//     N_PRESSED = 0x31,
//     N_RELEASED = 0xB1,
//     M_PRESSED = 0x32,
//     M_RELEASED = 0xB2,

//     ZERO_PRESSED = 0x29,
//     ONE_PRESSED = 0x2,
//     NINE_PRESSED = 0xA,

//     POINT_PRESSED = 0x34,
//     POINT_RELEASED = 0xB4,

//     SLASH_RELEASED = 0xB5,

//     BACKSPACE_PRESSED = 0xE,
//     BACKSPACE_RELEASED = 0x8E,
//     SPACE_PRESSED = 0x39,
//     SPACE_RELEASED = 0xB9,
//     ENTER_PRESSED = 0x1C,
//     ENTER_RELEASED = 0x9C,
// };

#endif
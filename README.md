# esd-project
Embedded System Design Project

The code was developed in the following series :

1. Visual Studio
   
   1.1. vs-initial (before optimizations)

   1.2. vs-optimized (after optimizations)

2. CodeWarrior

   2.1. cw-initial (before optimizations - same code with 1.1)

   2.2. cw-optimized (after optimizations - same code with 1.2)

The report can be found in greek and soon in english.

# Folder Structure

📦esd-project

┣ 📂Report

┃ ┣ 📂LaTeX-images

┃ ┣ 📂greek

┃ ┃ ┣ 📜Report.pdf

┃ ┃ ┗ 📜Report.docx

┣ 📂vs

┃ ┣ 📂vs-initial

┃ ┗ 📜vs-initial.c

┃ ┣ 📂vs-optimized

┃ ┃ ┗ 📜vs-optimized.c

┣ 📂cw

┃ ┣ 📂cw-initial

┃ ┗ 📜cw-initial.c

┃ ┣ 📂cw-optimized

┃ ┃ ┗ 📜cw-optimized.c

┗ 📜README.md

# Tools

 - [IDE - Metrowerks CodeWarrior for ARM Development Suite](http://www.metrowerks.com/)

 - [IDE - Visual Studio](https://visualstudio.microsoft.com/)

 - [LaTeX equations to Image online converter](https://latex2image.joeraut.com/)

 - [LaTeX in Visual Studio as Comments - VS Extension > Tools](https://marketplace.visualstudio.com/items?itemName=vs-publisher-1305558.VsTeXCommentsExtension2022)

 - [Using LaTeX in Visual Studio Code on Windows - Medium](https://guillaumeblanchet.medium.com/using-latex-in-visual-studio-code-on-windows-121032043dad)

 - [YUV Player](https://github.com/Tee0125/yuvplayer)

# Guides

 - [The big list of LaTeX symbols](https://ftp.cc.uoc.gr/mirrors/CTAN/info/symbols/comprehensive/symbols-a4.pdf)

# Papers

 - [Space Convolution Illustration](https://www.researchgate.net/figure/Schematic-illustration-of-a-convolutional-operation-The-convolutional-kernel-shifts-over_fig2_332190148)

# Articles

 - [General Introduction](https://en.wikipedia.org/wiki/Digital_image_processing)

 - [What is YUV?](https://www.pcmag.com/encyclopedia/term/yuv)

 - [RGB to YUV and their range of values](https://dexonsystems.com/blog/rgb-yuv-color-spaces#:~:text=Like%20any%20color%20space%2C%20YUV%20space%20uses%20mathematical,U%20and%20V%20range%20from%20-0.5%20to%200.5.)

 - [Conversion between YUV and RGB](https://www.pcmag.com/encyclopedia/term/yuvrgb-conversion-formulas)

 - [RGB to grayscale](https://www.baeldung.com/cs/convert-rgb-to-grayscale)

 - [Canny Edge Detector](https://en.wikipedia.org/wiki/Canny_edge_detector)

 - [What happened to Metrowerks?](https://theqalead.com/general/what-happened-to-metrowerks/)

 - [Memory Options for Embedded Systems](https://www.qt.io/embedded-development-talk/memory-options-for-embedded-systems-how-to-select-the-right-memory-configuration)

# Videos

 - [Memory in ARM Cortex-M](https://www.youtube.com/watch?v=aT5XMOrid7Y)

 - [Stack and Heap in Memory](https://www.youtube.com/watch?v=_8-ht2AKyH4)

# Blogs

 - [Video Engineering](https://poynton.ca/Poynton-video-eng.html)

# Repositories

 - Thanks to [Vincmazet](https://github.com/vincmazet) for the [Basics of Image Processing](https://vincmazet.github.io/bip/) website.

# Books

 - [Digital Image Processing 4th Edition by Rafael C. Gonzalez , Richard E. Woods](https://studylib.net/doc/25705174/digital-image-processing-4th-ed.---r.-gonzalez--r.-woods-...)

 - [Digital Video and HD Algorithms and Interfaces, Second Edition](https://shop.elsevier.com/books/digital-video-and-hd/poynton/978-0-12-391926-7)

 - [Engineering a Compiler](https://shop.elsevier.com/books/engineering-a-compiler/cooper/978-0-12-815412-0)

 - [Dragon Book](https://suif.stanford.edu/dragonbook/)

# Documentation

 - [ARM Developer - Learn the architecture -  Optimizing C code with Neon intrinsics](https://developer.arm.com/documentation/102467/0201?lang=en)

 - [ARM Developer Suite CodeWarrior IDE Guide](https://developer.arm.com/documentation/dui0065/d?lang=en)

 - [About ARM and Thumb Instructions of ARM7TDMI](https://microchip.my.site.com/s/article/ARM-and-Thumb-Instructions-of-ARM7TDMI)

 - [About Memories on ARM Cortex M - Chapter 8](https://github.com/arm-university/Fundamentals-of-System-on-Chip-Design-on-Arm-Cortex-M-Microcontrollers/tree/main)

 - [Learn about Thumb-2 ISA (T32 and T16)](https://developer.arm.com/Architectures/T32%20Instruction%20Set%20Architecture)

 - [ARM7TDMI Technical Reference Manual r4p1](https://developer.arm.com/documentation/ddi0210/c)

 - [Legacy __user_initial_stackheap() documentation](https://developer.arm.com/documentation/dui0492/i/Cihhdahf)

Since the ARM7TDMI is an implementation of the legacy 32-bit architecture armv4T, according to [ARM Developer Hub](https://learn.arm.com/learning-paths/embedded-systems/intro/background/) the recommended processors for embedded systems can be any of the following:

1. ARM Cortex-A for advanced OS

2. ARM Cortex-R for Real-Time Applications (RTOS)

3. ARM Cortex-M for MCUs

The supported architecture version is armv7, armv8, armv9 (latest at 2022).

CodeWarrior IDE vs ARM DS 2022 IDE :

- C Projects vs C/C++/Java/Python Projects

- ARM Executables vs ARM Bare-Metal Projects (and other)

- ARM Compiler vs ARM Compiler 6 (there is support for 5)

- AXD Debugger vs Debug on semihosting

- scatter.txt and memory.map supported on both

- Debugger Internals vs Trace (DTSL) and Streamline


# Tutorials for the ARM DS 2022

 - [Bare-Metal HelloWorld with scatter.txt - using commands - for armv8 on the FVP_Base_AEMvA which is a generic Arm Architecture platform, implementing 4 processors](https://learn.arm.com/learning-paths/embedded-systems/bare-metal/hello/)

 - [Design a dynamic Memory Allocator](https://learn.arm.com/learning-paths/cross-platform/dynamic-memory-allocator/)

 - 

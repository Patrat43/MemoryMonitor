# Memory Monitor
Abstract
---
This program is used to look for memory leaks in the program. By using custom macros and overloads, you can customize what objects/memory you want to track. During and after program compilation, execution, and closure, the memory used is tracked by this monitor.
These are tracked through 3 different outputs:
1. Output Console
2. Log File
3. CSV File (which can be converted to most spreadsheet programs)
---
Usage
---
To use this monitor in your program:
- Use the the macro `_new` to initialize objects (This allow this specific object to be tracked)
- Run your project and look at the `Output Console`
- After stopping your project, open the `root` of your project and find the `Log\` folder which will have both `.txt` & `.csv` log files

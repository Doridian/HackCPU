| Index | Name            | Write | Read | (Re)SetPos | Length   | GetPos |
|-------|-----------------|-------|------|------------|----------|--------|
| 0     | stdout (return) | Y     | Y    | I          | columns  | Y      |
| 1     | ROM             | N     | Y    | Y          | Y        | Y      |
| 2     | stdin (args.in) | N     | Y    | Y          | Y        | Y      |
| 3     | /dev/zero       | I     | 0x00 | I          | 0x00     | 0x00   |

Legend:
- Y = Works normally
- N = Errors when used
- I = Has no effect, but doesn't error
- Other value: Returns mentioned value

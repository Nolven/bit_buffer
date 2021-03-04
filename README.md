# How it works

Basically it's a bit buffer without padding. By this i mean that when we sequentially write values, 
they are stored in sequence without trailing 0, for example we can create a simple buffer and store 3 at first 8 bits:
```c++
    BinaryBuffer buff(5);
    buff.append(3, 8);
```
After it the buffer would look like (`'` stands for the end of a byte):  
`v------v - 3 is stored there`  
`00000101'00000000'00000000'00000000'00000000`

Simple, right? Let's add another value:
```c++
    buff.append(5, 3);
```
The resulting buffer is:  
`v--3---v'v5v`  
`00000101'11100000'00000000'00000000'00000000`  
By calling append with the length of 3 we wrote only meaningful bits into the buffer.  
So when we are going to read from it, we will simply call:
```c++
    buff.get<int8_t>(8, 3);
```
output bits will be interpreted as `00000111`

# Example //so useful much wow

For example you have some internal DTO like one below:

```c++
struct DTO
{
    int8_t  counter;
    int8_t  state;
    bool    flag1;
    bool    flag2;
    int8_t  value;
    int32_t greaterValue;
}
```

that must be transfered using binary UDP protocol like:

|    Byte | Bits  | Field name |  Note |
|   ---   |  ---  |     ---    |  ---  |
|    0    |  0-3  |   Counter  |   1   |
|    0    |  4-5  |   State    |   2   |
|    0    |  6    |   Flag1    |   3   |
|    0    |  7    |   Flag2    |   4   |
|    1    |  0-7  |   Value    |   5   |
| 2,3,4,5 |  0-7  |greaterValue|   5   |

So you need to package all these values without padding to one package.  
The easiest way to do it is just append values one by one:
```c++
    BinaryBuffer buff(6);

    buff.append<int8_t>(dto.counter, 3);
    buff.append<int8_t>(dto.state, 3);
    buff.append<int8_t>(dto.flag1, 1);
    buff.append<int8_t>(dto.flag2, 1);
    buff.append<int8_t>(dto.value, 7);
    buff.append<int32_t>(dto.greaterValue, 32);
```
Or if you want more readable way, then use insert instead of append:
```c++
    buff.insert(dto.counter, 0, 4);
    buff.insert(dto.state, 4, 2);
    buff.insert(dto.flag1, 6, 1);
    buff.insert(dto.flag2, 7, 1);
    buff.insert(dto.value, 8, 8);
    buff.insert(dto.greaterValue, 16, 32);
```

// float foo(int a);
int main(int argc, char* argv[])
{
    int x=5,z;
    int y = 2;
    x = y;
    x = (y + 1);
    z = x - 4;
    int k = z*8/2;
    return k*(-1*+k);
}

// int plus(int a, int b)
// {
//     return 0;
//     int x;
//     int y = 2;
//     x = y + 1;
//     int z = y-4;
//     return x;
// }

/*************************************************
*                   STRUCTURES
*  1. Struct definition
*  2. Struct forward declaration
*  3. Unnamed struct definition (Look CLASSES)
*************************************************/

// // 1.
// struct MyCoolStruct
// {
//     int mem1;
//     float mem2;
// };

// // 2.
// struct MyCoolForwardDeclaredStruct;

// /*************************************************
// *                   UNIONS
// *  1. Union definition
// *  2. Unnamed union definition
// *************************************************/

// union MyCoolUnion
// {
//     int mem1;
//     float mem2;
// };

// static union
// {

// };

// /*************************************************
// *                   CLASSES
// *  1. Class definition
// *  2. Class forward declaration
// *  3. Unnamed class definition
// *************************************************/
// // 1.
// class DummyClass
// {
//     struct//unnamed struct
//     {
//         int mem1;
//         float mem2;
//     };

//     //3.
//     class
//     {
//     public:
//         int s;
//     };
// };

// // 2.
// class DummyClassFwdDecl;

// /*************************************************
// *                   ENUMS
// *  1. Enum
// *  2. Enum class
// *  3. Enum struct
// *************************************************/

// enum MyEnum
// {

// };

// enum MyEnumFwdRef;

// enum class MyEnumClass
// {

// };

// enum class MyEnumClassFwdRef;

// enum struct MyEnumStruct
// {

// };

// enum struct MyEnumStructFwdRef;

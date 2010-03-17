#ifndef ENU_H
#define ENU_H

namespace Enu
{
   // Instruction mnemonics
   enum Connection
   {
        GET, JOIN, LEAVE,
    };

   enum FindMode
   {
       Contains, StartsWith, EntireWord,
   };

   enum Permissions {
       ReadWrite = 2,
       ReadOnly = 1,
       Waiting = 0,

       Owner = 1,
   };

}

#endif // ENU_H

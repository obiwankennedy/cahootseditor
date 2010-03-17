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

}

#endif // ENU_H

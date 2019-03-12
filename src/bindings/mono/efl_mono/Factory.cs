using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.ComponentModel;

#if EFL_BETA

namespace Efl { namespace Ui {

public class ItemFactory<T> : Efl.Ui.CachingFactory, IDisposable
{
    public ItemFactory(Efl.Object parent = null)
        : base (parent, typeof(T))
    {
    }

    
}

} }

#endif // EFL_BETA

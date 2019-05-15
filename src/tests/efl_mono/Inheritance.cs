using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

using EinaTestData;
using static EinaTestData.BaseData;

namespace TestSuite
{

internal class Rubbish
{
    public int Content { get; set; } = 0;
}

class TestInheritance
{
    internal class Inherit1 : Dummy.TestObject
    {
        override public void IntOut (int x, out int y)
        {
            y = 10*x;
        }
    }

    internal class Inherit2 : Dummy.TestObject, Dummy.IInheritIface
    {
        override public void IntOut (int x, out int y)
        {
            Console.WriteLine("IntOut");
            y = 10*x;
        }

        public string StringshareTest (string i)
        {
            Console.WriteLine("StringshareTest");
            return "Hello World";
        }
    }

    internal class Inherit3Parent : Dummy.TestObject
    {
        public bool disposed = false;
        public bool childDisposed = false;

        ~Inherit3Parent()
        {
            Console.WriteLine("finalizer called for parent");
        }

        protected override void Dispose (bool disposing)
        {
            Console.WriteLine("Dispose parent");
            base.Dispose(disposing);
        }
    }

    internal class Inherit3Child : Dummy.TestObject
    {
        Inherit3Parent parent;
        public Inherit3Child (Inherit3Parent parent) : base (parent)
        {
            this.parent = parent;
        }

        ~Inherit3Child()
        {
            Console.WriteLine("finalizer called for child");
        }

        protected override void Dispose (bool disposing)
        {
            parent.childDisposed = true;
            Console.WriteLine("Dispose parent");
            base.Dispose(disposing);
        }
    }

    public static void test_inherit_from_regular_class()
    {
        var obj = new Inherit1();
        int i = Dummy.InheritHelper.ReceiveDummyAndCallIntOut(obj);
        Test.AssertEquals (50, i);
    }

    public static void test_inherit_from_iface()
    {
        var obj = new Inherit2();
        int i = Dummy.InheritHelper.ReceiveDummyAndCallIntOut(obj);
        Test.AssertEquals (50, i);
        string s = Dummy.InheritHelper.ReceiveDummyAndCallInStringshare(obj);
        Test.AssertEquals ("Hello World", s);
    }

    private static void MakeRubbish(out WeakReference objWRef)
    {
        var obj = new Inherit3Parent();

        objWRef = new WeakReference(obj);


        int n = 2111000;
        var arr1 = new Rubbish[n];
        var arr2 = new Rubbish[n];
        var rand = new Random();
        for (int i = 0; i != n; ++i)
        {
            arr1[i] = new Rubbish();
            arr1[i].Content = rand.Next();
        }

        for (int i = n-1; i != -1; --i)
        {
            arr2[i] = new Rubbish();
            arr2[i].Content = arr1[(i+n/2)%n].Content + (arr1[i].Content%2 == 0 ? 1 : -1);
        }

        var x = rand.Next(n);
        Console.WriteLine($"arr1[x] = {arr1[x].Content}; arr2[x] = {arr2[x].Content}");
    }

    private static void hsasdgsdahd(out WeakReference parentWRef, out WeakReference childWRef)
    {
        //var parent = new Inherit3Parent();
        //var child = new Inherit3Child(parent);

        var parent = new Dummy.TestObject();
        var child = new Dummy.TestObject(parent);

        parentWRef = new WeakReference(parent);
        childWRef = new WeakReference(child);

        Console.WriteLine($"MMMMM Parent [{parent.ToString()}] has {Efl.Eo.Globals.efl_ref_count(parent.NativeHandle)} refs");
        Console.WriteLine($"MMMMM Child [{child.ToString()}] has {Efl.Eo.Globals.efl_ref_count(child.NativeHandle)} refs");

        child = null;

        System.GC.Collect(System.GC.MaxGeneration, GCCollectionMode.Forced, true, true);
        System.GC.WaitForPendingFinalizers();
        Efl.App.AppMain.Iterate();

        child = (Dummy.TestObject) childWRef.Target;

        Test.AssertNotNull(parent);
        Test.AssertNotNull(child);
        //Test.AssertEquals(false, parent.disposed);
        //Test.AssertEquals(false, parent.childDisposed);

        Console.WriteLine($"MMMMM Parent [{parent.ToString()}] has {Efl.Eo.Globals.efl_ref_count(parent.NativeHandle)} refs");
        Console.WriteLine($"MMMMM Child [{child.ToString()}] has {Efl.Eo.Globals.efl_ref_count(child.NativeHandle)} refs");

        parent = null;
        child = null;
    }

    public static void test_inherit_lifetime()
    {
        WeakReference parentWRef;
        WeakReference childWRef;

        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();
        Efl.App.AppMain.Iterate();

        hsasdgsdahd(out parentWRef, out childWRef);

        WeakReference objWRef;
        MakeRubbish(out objWRef);

        for (int i = 0; i < 10; ++i)
        {
            for (int j = 0; j < 1000; ++j)
            {
                System.GC.Collect();
            }

            System.GC.WaitForPendingFinalizers();
            Efl.App.AppMain.Iterate();
        }

        var obj = (Rubbish) objWRef.Target;
        var parent = (Dummy.TestObject) parentWRef.Target;
        var child = (Dummy.TestObject) childWRef.Target;

        Test.AssertNull(obj);
        Test.AssertNull(parent);
        Test.AssertNull(child);
    }
}

}

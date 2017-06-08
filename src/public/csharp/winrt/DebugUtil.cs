/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DebugUtil.cs
****/
using System;

namespace Microsoft.PropertyModel
{
#if DEBUG
    class DebugUtilInstance
    {
        private ICommandModel _debugUtilCommand;

        internal DebugUtilInstance(ICommandModel debugUtilCommand)
        {
            this._debugUtilCommand = debugUtilCommand;
        }

        internal void ClearTrackedObjects()
        {
            _debugUtilCommand.Execute((uint)0);
        }
        internal void DumpTrackedObjects()
        {
            _debugUtilCommand.Execute((uint)1);
        }
        internal int CountTrackedObjects()
        {
            return (int)_debugUtilCommand.Execute((uint)2);
        }
        internal uint GetOptionsFlags()
        {
            return (uint)_debugUtilCommand.Execute((uint)3);
        }
        internal void SetOptionsFlags(uint flags)
        {
            _debugUtilCommand.Execute(new uint[]{3,flags});
        }
        internal void SetOptionsFlag(uint flags)
        {
            _debugUtilCommand.Execute(new uint[] { 4, flags });
        }
        internal void RemoveOptionsFlags(uint flags)
        {
            _debugUtilCommand.Execute(new uint[] { 5, flags });
        }
        internal bool IsOptionFlag(uint flags)
        {
            return (bool)_debugUtilCommand.Execute(new uint[] { 6, flags });
        }
        internal Tuple<uint,ulong> GetPerfCounter(uint perfId)
        {
            object[] values = (object[])_debugUtilCommand.Execute(new uint[] { 7, perfId });
            return new Tuple<uint, ulong>((uint)values[0], (ulong)values[1]);
        }
        internal void ClearPerfCounter(uint perfId)
        {
           _debugUtilCommand.Execute(new uint[] { 8, perfId });
        }
        internal void AddPerfCounter(uint perfId,ulong time)
        {
            _debugUtilCommand.Execute(new object[] { 9, perfId, time });
        }
        internal void ClearAllPerfCounters()
        {
            _debugUtilCommand.Execute((uint)10);
        }
        internal IDictionary GetPerfCountersInfo()
        {
            return _debugUtilCommand.Execute((uint)11) as IDictionary;
        }
        internal void IsAssetDisabled(bool value)
        {
            if (value)
                SetOptionsFlag(100);
            else
                RemoveOptionsFlags(100);
        }
    }

    static class DebugUtil
    {
        private static DebugUtilInstance _debugUtilInstance;
        private static Guid _debugUtilIId;

        static DebugUtil()
        {
            _debugUtilIId = new Guid("71D56E14-BC19-4687-98C5-DE9EBEF5A0E0");
            ICommandModel debugUtilCommand = PropertyModelInstance.ObjectFactory.TryCreateObject(
                _debugUtilIId,
                null) as ICommandModel;
            System.Diagnostics.Debug.Assert(debugUtilCommand != null);
            _debugUtilInstance = new DebugUtilInstance(debugUtilCommand);
        }
        static internal Guid DebugUtilCommandIId
        {
            get
            {
                return _debugUtilIId;
            }
        }

        static internal void ClearTrackedObjects()
        {
            _debugUtilInstance.ClearTrackedObjects();
        }
        static internal void DumpTrackedObjects()
        {
            _debugUtilInstance.DumpTrackedObjects();
        }
        static internal int CountTrackedObjects()
        {
            return _debugUtilInstance.CountTrackedObjects();
        }
        static internal uint GetOptionsFlags()
        {
            return _debugUtilInstance.GetOptionsFlags();
        }
        static internal void SetOptionsFlags(uint flags)
        {
            _debugUtilInstance.SetOptionsFlags(flags);
        }
        static internal void SetOptionsFlag(uint flags)
        {
            _debugUtilInstance.SetOptionsFlag(flags);
        }
        static internal void RemoveOptionsFlags(uint flags)
        {
            _debugUtilInstance.RemoveOptionsFlags(flags);
        }
        static internal bool IsOptionFlag(uint flags)
        {
            return _debugUtilInstance.IsOptionFlag(flags);
        }
        static internal Tuple<uint,ulong> GetPerfCounter(uint perfId)
        {
            return _debugUtilInstance.GetPerfCounter(perfId);
        }
        static internal void ClearPerfCounter(uint perfId)
        {
           _debugUtilInstance.ClearPerfCounter(perfId);
        }
        static internal void AddPerfCounter(uint perfId,ulong time)
        {
            _debugUtilInstance.AddPerfCounter(perfId,time);
        }
        static internal void ClearAllPerfCounters()
        {
            _debugUtilInstance.ClearAllPerfCounters();
        }
        static internal IDictionary GetPerfCountersInfo()
        {
            return _debugUtilInstance.GetPerfCountersInfo();
        }
        static internal void IsAssetDisabled(bool value)
        {
            _debugUtilInstance.IsAssetDisabled(value);
        }
    }
#endif
}

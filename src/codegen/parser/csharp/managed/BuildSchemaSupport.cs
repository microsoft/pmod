/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BuildSchemaSupport.cs
****/
using System;

namespace PropertyModel
{
    /// <summary>
    /// IDictionary interface
    /// </summary>
    public interface IDictionary
    {
    }

    /// <summary>
    /// IObject interface
    /// </summary>
    public interface IObject
    {
    }

    /// <summary>
    /// IObservableObject interface
    /// </summary>
    public interface IObservableObject
    {
    }

    /// <summary>
    /// ICommand interface
    /// </summary>
    public interface ICommand
    {
    }

    /// <summary>
    /// IEnumeration interface
    /// </summary>
    public interface IEnumerable
    {
    }

    /// <summary>
    /// ICollection interface
    /// </summary>
    public interface ICollection
    {
    }

    /// <summary>
    /// IList interface
    /// </summary>
    public interface IList
    {
    }

    /// <summary>
    /// Enum offset Attribute Class
    /// </summary>
    [AttributeUsage(AttributeTargets.Enum)]
    public sealed class EnumOffsetAttribute : Attribute
    {
        /// <summary>
        /// Offset to be added into the enumeration values
        /// </summary>
        public uint Offset { get; set; }
    }

    /// <summary>
    /// Enum base attribute class
    /// </summary>
    [AttributeUsage(AttributeTargets.Enum)]
    public sealed class EnumBaseTypeAttribute : Attribute
    {
        /// <summary>
        /// The Base Type beign referenced
        /// </summary>
        public Type BaseType { get; set; }
    }

    /// <summary>
    /// Property Model Attribute Class
    /// </summary>
    [AttributeUsage(AttributeTargets.Property)]
    public sealed class PropertyAttribute : Attribute
    {
        /// <summary>
        /// Options flags to support custom behaviors
        /// </summary>
        public int OptionFlags { get; set; }

        /// <summary>
        /// If Cache can be enabled for this property
        /// </summary>
        public bool IsCacheEnabled { get; set; }

        /// <summary>
        /// If the property is related to be the 'Parent' of this model
        /// </summary>
        public bool IsParent { get; set; }

        /// <summary>
        /// If the property is related to be an Ancestor of this model
        /// </summary>
        public bool IsAncestor { get; set; }

        /// <summary>
        /// If the property is a property binding enabled we can specify an object SourcePath.
        /// </summary>
        public int RuntimeClassOptions { get; set; }

        /// <summary>
        /// If the property is a property binding enabled we can specify an object SourcePath.
        /// </summary>
        public string BindingSourcePath { get; set; }

        /// <summary>
        /// If the property is stored as a Weak reference
        /// </summary>
        public bool IsWeakReference { get; set; }

        /// <summary>
        /// If the property is related to enable acitvation for this property.
        /// </summary>
        public bool IsRuntimeActivated { get; set; }

        /// <summary>
        /// If the property is reserved for private use
        /// </summary>
        public bool IsReserved { get; set; }

        /// <summary>
        /// If the property is a custom container for fast model implementation
        /// </summary>
        public bool IsCustomContainer { get; set; }

        /// <summary>
        /// Empty constructor
        /// </summary>
        public PropertyAttribute()
        {
        }
    }

    /// <summary>
    /// Attribute class for modelling custom property attributes
    /// </summary>
    [AttributeUsage(AttributeTargets.Property, AllowMultiple = true)]
    public sealed partial class PropertyAttributeEx : Attribute
    {
        /// <summary>
        /// Empty constructor
        /// </summary>
        public PropertyAttributeEx()
        {
        }
    }

    /// <summary>
    /// Command Model Attribute Class
    /// </summary>
    [AttributeUsage(AttributeTargets.Property)]
    public sealed class CommandAttribute : Attribute
    {
        /// <summary>
        /// The Command Id Type
        /// </summary>
        public int CommandId { get; set; }

        /// <summary>
        /// The Parameter Type
        /// </summary>
        public Type Parameter { get; set; }

        /// <summary>
        /// The Result Type
        /// </summary>
        public Type Result { get; set; }

        /// <summary>
        /// The Source Type
        /// </summary>
        public Type SourceType { get; set; }

        /// <summary>
        /// If Auto Generate
        /// </summary>
        public bool IsAutoGenerate { get; set; }

        /// <summary>
        /// If Async
        /// </summary>
        public bool IsAsync { get; set; }

        /// <summary>
        /// If Unknown State
        /// </summary>
        public bool IsUnknownState { get; set; }

        /// <summary>
        /// ctor
        /// </summary>
        public CommandAttribute()
        {
        }
    }

    /// <summary>
    /// Command Model Attribute Class
    /// </summary>
    [AttributeUsage(AttributeTargets.Method)]
    public sealed class MethodAttribute : Attribute
    {
        /// <summary>
        /// If Async
        /// </summary>
        public bool IsAsync { get; set; }
    }

    /// <summary>
    /// Enumeration Attribute Class
    /// </summary>
    [AttributeUsage(AttributeTargets.Property | AttributeTargets.Interface)]
    public class EnumerationAttribute : Attribute
    {
        /// <summary>
        /// Item type
        /// </summary>
        public Type ItemType { get; set; }

        /// <summary>
        /// ctor
        /// </summary>
        public EnumerationAttribute()
        {
        }
    }

    /// <summary>
    /// Collection Model Attribute Class
    /// </summary>
    [AttributeUsage(AttributeTargets.Property | AttributeTargets.Interface)]
    public sealed class CollectionAttribute : EnumerationAttribute
    {

        /// <summary>
        /// ctor
        /// </summary>
        public CollectionAttribute()
        {
        }
    }

    /// <summary>
    /// Model Attribute Class
    /// </summary>
    [AttributeUsage(AttributeTargets.Interface)]
    public sealed class ModelAttribute : Attribute
    {
        /// <summary>
        /// The Source Type
        /// </summary>
        public Type SourceType { get; set; }

        /// <summary>
        /// This will be the custom interface name that will be generated
        /// </summary>
        public string CustomInterfaceName { get; set; }

        /// <summary>
        /// This marks the custom interface as part of the Private interface inheritance
        /// </summary>
        public bool IsCustomPrivateInterface { get; set; }

        /// <summary>
        /// This property is used to inherit this attribute in all the define properties for the model.
        /// This will affect all the IsCustomContainer as if it was define per property.
        /// </summary>
        public bool IsCustomContainer { get; set; }

        /// <summary>
        /// This property is used to inherit this attribute in all the define properties for the model.
        /// This will affect all the RuntimeClassOptions as if it was define per property.
        /// </summary>
        public int RuntimeClassOptions { get; set; }

        /// <summary>
        /// If force this interface to be a Property Data structure
        /// </summary>
        public bool IsObservableObject { get; set; }

        /// <summary>
        /// ctor
        /// </summary>
        public ModelAttribute()
        {
        }
    }

    /// <summary>
    /// Event Attribute Class
    /// </summary>
    [AttributeUsage(AttributeTargets.Event)]
    public sealed class EventAttribute : Attribute
    {
        /// <summary>
        /// Event arguments
        /// </summary>
        public Type EventArgs { get; set; }

        /// <summary>
        /// ctor
        /// </summary>
        public EventAttribute()
        {
        }
    }

}

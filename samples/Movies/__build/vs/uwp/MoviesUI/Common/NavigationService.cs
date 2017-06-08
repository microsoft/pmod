/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NavigationService.cs
****/

using System;
using System.Collections.Generic;
using System.Diagnostics;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace MoviesUI.Common
{
    public class NavigationService
    {
        private readonly Frame _frame;

        /// <summary>
        /// TODO: remove precautions if possible after knowing more about the API:
        /// We store the manager in order to unsubscribe from it and then get a new one and subscribe to new one.
        /// This is due to lack of knowledge of lifetime of the manager. It's not clear from MSDN will the
        /// SystemNavigationManager.GetForCurrentView() call always return same instance for application lifetime
        /// (including suspend/resume) or not. So to be very cautious we assume we do not know anything about
        /// lifetime of the manager. When we will know more about the lifetime we can remove all/most of this
        /// caching and sub/unsub code.
        /// </summary>
        private SystemNavigationManager _systemNavigationManager;

        /// <summary>
        /// The frame on which navigation service is acting on.
        /// </summary>
        protected Frame Frame => _frame;


        /// <summary>
        /// Notify when the frame detects the back button should be visible or not
        /// </summary>
        public event EventHandler CanGoBackChanged;


        /// <summary>
        /// Initializes a new instance of the <see cref="NavigationServiceBase"/> class
        /// </summary>
        /// <param name="frame">The application frame</param>
        public NavigationService(Frame frame)
        {
            _frame = frame;

            _frame.Navigating += FrameOnNavigating;
            _frame.Navigated += FrameOnNavigated;
            _frame.NavigationFailed += FrameOnNavigationFailed;
            _frame.NavigationStopped += FrameOnNavigationStopped;
        }

        private void FrameOnNavigating(object sender, NavigatingCancelEventArgs navigatingCancelEventArgs)
        {
            UnsubscribeFromSystemNavigationManager();
        }

        private void FrameOnNavigationStopped(object sender, NavigationEventArgs navigationEventArgs)
        {
            SubscribeToSystemNavigationManager();
        }

        private void FrameOnNavigated(object sender, NavigationEventArgs navigationEventArgs)
        {
            SubscribeToSystemNavigationManager();
        }

        void FrameOnNavigationFailed(object sender, NavigationFailedEventArgs e)
        {
            if (Debugger.IsAttached)
            {
                Debugger.Break();
            }

            SubscribeToSystemNavigationManager();
        }

        private void UnsubscribeFromSystemNavigationManager()
        {
            if (_systemNavigationManager != null)
            {
                _systemNavigationManager.BackRequested -= SystemNavigationManager_BackRequested;
                _systemNavigationManager = null;
            }
        }

        private void SubscribeToSystemNavigationManager()
        {
            UnsubscribeFromSystemNavigationManager();
            _systemNavigationManager = SystemNavigationManager.GetForCurrentView();
            _systemNavigationManager.BackRequested += SystemNavigationManager_BackRequested;
            UpdateBackButton();
        }

        public void SuspendBackButtonRequestedEvents()
        {
            UnsubscribeFromSystemNavigationManager();
        }

        public void ResumeBackButtonRequestedEvents()
        {
            SubscribeToSystemNavigationManager();
        }

        private void SystemNavigationManager_BackRequested(object sender, BackRequestedEventArgs e)
        {
            // We should never set the handled if we cant go back or if 
            // we are in the root it will prevent exit in phone.
            if (e.Handled == false && CanGoBack())
            {
                e.Handled = true;
                GoBack();
            }
        }

        protected void UpdateBackButton()
        {
            SystemNavigationManager.GetForCurrentView().AppViewBackButtonVisibility =
                CanGoBack() ? AppViewBackButtonVisibility.Visible : AppViewBackButtonVisibility.Collapsed;

            CanGoBackChanged?.Invoke(this, null);
        }

        /// <summary>
        /// Determine if the <see cref="Frame"/> can go back.
        /// </summary>
        /// <returns>
        /// true if the <see cref="Frame"/> has at least one entry 
        /// in the back navigation history.
        /// </returns>
        public bool CanGoBack()
        {
            return this.Frame != null && this.Frame.CanGoBack;
        }

        /// <summary>
        /// Determine if the <see cref="Frame"/> can go forward.
        /// </summary>
        /// <returns>
        /// true if the <see cref="Frame"/> has at least one entry 
        /// in the forward navigation history.
        /// </returns>
        public bool CanGoForward()
        {
            return this.Frame != null && this.Frame.CanGoForward;
        }

        /// <summary>
        /// Invokes the <see cref="Windows.UI.Xaml.Controls.Frame.GoBack"/> method.
        /// </summary>
        public void GoBack()
        {
            if (this.CanGoBack())
            {
                this.Frame.GoBack();
            }
        }

        /// <summary>
        /// Invokes the <see cref="Windows.UI.Xaml.Controls.Frame.GoForward"/> method.
        /// </summary>
        public void GoForward()
        {
            if (this.CanGoForward())
            {
                this.Frame.GoForward();
            }
        }

        /// <summary>
        /// Clears the back stack.
        /// </summary>
        public void ClearBackStack()
        {
            if ((this.Frame != null) && (this.Frame.BackStackDepth > 0))
            {
                this.Frame.BackStack.Clear();
                UpdateBackButton();
            }
        }

        /// <summary>
        /// Removes the top of back stack.
        /// </summary>
        public void RemoveBackEntry()
        {
            if ((this.Frame != null) && (this.Frame.BackStackDepth > 0))
            {
                this.Frame.BackStack.RemoveAt(this.Frame.BackStackDepth - 1);
                UpdateBackButton();
            }
        }

        /// <summary>
        /// Get the type of the previous page, if any.
        /// </summary>
        /// <returns>If there is a back entry, returns the type of that entry. Otherwise, returns null.</returns>
        public Type GetBackEntryType()
        {
            Type pageType = null;

            if ((this.Frame != null) && (this.Frame.BackStackDepth > 0))
            {
                pageType = this.Frame.BackStack[this.Frame.BackStackDepth - 1].SourcePageType;
            }

            return pageType;
        }

        /// <summary>
        /// Navigates to the root page of the frame (the first page
        /// in the navigation history)
        /// </summary>
        public void NavigateToRootPage()
        {
            while (this.CanGoBack())
            {
                this.GoBack();
            }
        }
        
      

        /// <summary>
        /// Invokes navigating to the specified page by type asynchronously on the UI thread.
        /// </summary>
        /// <param name="sourcePageType">The page type to navigate to.</param>
        /// <param name="parameters">The dictionary of parameters to send to the page.</param>
        /// <returns>True is navigation succeeded</returns>
        protected void NavigateToPageRunAsync(Type sourcePageType, IDictionary<string, object> parameters)
        {
#pragma warning disable 4014 // We have to return control to UI, so that it can close current content dialog.
            Window.Current.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => NavigateToPage(sourcePageType, parameters));
#pragma warning restore 4014
        }

        /// <summary>
        /// Navigate to the specified page by type while allowing duplicate pages of this type on stack.
        /// </summary>
        /// <param name="sourcePageType">The page type to navigate to.</param>
        /// <param name="parameters">The dictionary of parameters to send to the page.</param>
        /// <returns>True is navigation succeeded</returns>
        protected bool NavigateToPage(Type sourcePageType, IDictionary<string, object> parameters)
        {
            return NavigateToPage(sourcePageType, parameters, true);
        }

        /// <summary>
        /// Navigate to the specified page by type.
        /// </summary>
        /// <param name="sourcePageType">The page type to navigate to.</param>
        /// <param name="parameters">The dictionary of parameters to send to the page.</param>
        /// <param name="removeDuplicates">Is a duplicate page on the stack allowed?</param>
        /// <returns>True is navigation succeeded</returns>
        protected bool NavigateToPage(Type sourcePageType, IDictionary<string, object> parameters, bool removeDuplicates)
        {
            bool succeeded = false;

            if ((this.Frame.CurrentSourcePageType == sourcePageType) &&
                (null == parameters))
            {
                succeeded = true;
            }
            else
            {
                succeeded = this.Frame.Navigate(sourcePageType, parameters);

                if (removeDuplicates)
                {
                    FindAndCollapseDuplicatesInTheBackStack();
                }
            }

            return succeeded;
        }

        /// <summary>
        /// Search the Backstack from the bottom looking for a duplicate of the current page.
        /// If a duplicate is found, remove that plus everything above. At the end the current page
        /// should sit where the duplicate was. 
        /// 
        /// For example:
        /// 
        /// currentPage  ConversationPage
        /// backStack[2] contactPage
        /// backStack[1] ConversationPage
        /// backStack[0] HomePage
        /// 
        /// will be collapsed like this:
        /// 
        /// currentPage  ConversationPage
        /// backStack[0] HomePage
        /// 
        /// </summary>
        /// <returns>true if a duplicate is found</returns>
        private bool FindAndCollapseDuplicatesInTheBackStack()
        {
            Type currentPageType = this.Frame.CurrentSourcePageType;
            bool foundDuplicate = false;

            // Check if the current page already exist in the backstack.
            for (int iIfEqualsCurrent = 0; iIfEqualsCurrent < this.Frame.BackStackDepth; iIfEqualsCurrent++)
            {
                PageStackEntry page = this.Frame.BackStack[iIfEqualsCurrent];
                if (page.SourcePageType == currentPageType)
                {
                    // Remove this and all the backstack above.
                    for (int jToRemove = (this.Frame.BackStackDepth - 1); jToRemove >= iIfEqualsCurrent; jToRemove--)
                    {
                        this.Frame.BackStack.RemoveAt(jToRemove);
                    }
                    UpdateBackButton();

                    foundDuplicate = true;
                    break;
                }
            }

            return foundDuplicate;
        }
    }
}

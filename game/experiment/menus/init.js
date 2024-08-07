/**
 * This is a mock for when you're working in the browser instead of in-game.
 * It installs the same g_Interop the game would and calls the interop:ready event
 * on the window.
 */
document.addEventListener('DOMContentLoaded', function () {
    if (!navigator.userAgent.includes('Valve Client')) {
        window.g_Interop = window.g_Interop || {};
        window.g_Interop.__callbackQueue = window.g_Interop.__callbackQueue || [];

        window.g_Interop.__isReady = window.g_Interop.__isReady || false;

        if (!window.g_Interop.__isReady) {
            window.g_Interop.DispatchReadyEvent = function () {
                if (window.g_Interop.__isReady) return;
                window.g_Interop.__isReady = true;
                window.dispatchEvent(new Event('interop:ready'));
            };

            window.addEventListener('error', function (e) {
                //alert('%s' + JSON.stringify(e));
            });

            (function () {
                let oldLog = console.log;
                console.log = function () {
                    oldLog.apply(console, arguments);
                    //alert('%s' + JSON.stringify({ type: 'log', arguments: Array.prototype.slice.call(arguments) }));
                };
            })();
        }

        window.dispatchEvent(new Event('interop:installmock'));

        window.g_Interop.DispatchReadyEvent();
    }
});

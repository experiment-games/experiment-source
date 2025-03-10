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

let loadingOverlay;

document.addEventListener('DOMContentLoaded', function () {
  loadingOverlay = document.createElement('div');
  loadingOverlay.classList.add('hidden', 'inset-0', 'fixed', 'bg-black', 'bg-opacity-50', 'flex', 'items-center', 'justify-center', 'z-50');
  loadingOverlay.innerHTML = `
    <div class="flex flex-col gap-2 items-center">
      <div class="text-2xl text-white" id="loadingText">Loading...</div>
      <svg class="animate-spin -ml-1 mr-3 h-5 w-5 text-white" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
        <circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle>
        <path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
      </svg>
    </div>
  `;
  document.body.appendChild(loadingOverlay);
});

function showLoadingOverlay(text, callback) {
  const loadingText = loadingOverlay.querySelector('#loadingText');
  loadingText.textContent = text || 'Loading...';

  loadingOverlay.classList.remove('hidden');

  const finish = () => {
    hideLoadingOverlay();
  }

  callback(finish);
}

function hideLoadingOverlay() {
  loadingOverlay.classList.add('hidden');
}

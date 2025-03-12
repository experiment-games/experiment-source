const BACKGROUND_CYCLE_INTERVAL_MS = 10000;

const BACKGROUND_INITIAL = 0;
const BACKGROUND_LOADING = 1;
const BACKGROUND_MAINMENU = 2;
const BACKGROUND_LEVEL = 3;
const BACKGROUND_DISCONNECTED = 4;

const backgrounds = ['outland.jpg', 'breens-office.jpg'];
let backgroundIndex = Math.floor(Math.random() * backgrounds.length);

const containerEl = document.getElementById('background-container');

const backgroundEl = document.createElement('div');
backgroundEl.classList.add('background');
backgroundEl.style.backgroundImage = `url('images/backgrounds/${backgrounds[backgroundIndex]}')`;

const backgroundBufferEl = document.createElement('div');
backgroundBufferEl.classList.add('background-next');

containerEl.append(backgroundEl, backgroundBufferEl);

function changeBackground() {
  backgroundIndex = (backgroundIndex + 1) % backgrounds.length;
  backgroundBufferEl.style.backgroundImage = `url('images/backgrounds/${backgrounds[backgroundIndex]}')`;
  backgroundBufferEl.style.opacity = '1';

  setTimeout(() => {
    backgroundEl.style.backgroundImage = backgroundBufferEl.style.backgroundImage;
    backgroundBufferEl.style.opacity = '0';
  }, 1000);
}

setInterval(changeBackground, BACKGROUND_CYCLE_INTERVAL_MS);

function SetBackgroundRenderState(state) {
  if (state === BACKGROUND_LEVEL) {
    document.body.classList.add('in-level');
  } else {
    document.body.classList.remove('in-level');
  }
}

function ServerListComplete(wasSuccessful, errorMessage) {
  // console.log('Server list complete:', wasSuccessful, errorMessage);
}

function ServerListFailed(errorMessage) {
  console.log('Server list failed:', errorMessage);
}

const serverBrowserItemTemplate = document.querySelector('#serverBrowserItemTemplate');

function ServerListAdd(info) {
  const serverListContainer = document.getElementById('server-list-container');
  const serverList = serverListContainer.querySelector('#content-' + info.serverTab.toLowerCase());

  if (!serverList) {
    console.error('Server list not found for tab:', info.serverTab);
    return;
  }

  const serverListTable = serverList.querySelector('table tbody');

  let serverAddress = info.serverAddress;

  function setServerItemData(serverItem) {
    const players = info.serverPlayers;
    const maxPlayers = info.serverMaxPlayers;
    const ping = info.serverPing;
    const nameEl = serverItem.querySelector('.server-name');

    if (info.serverHostName) {
      nameEl.textContent = info.serverHostName;
    } else {
      nameEl.textContent = `Loading ${serverAddress}...`;
    }

    serverItem.querySelector('.server-players').textContent = players ? `${players}/${maxPlayers}` : '';
    serverItem.querySelector('.server-map').textContent = info.serverMap ?? '';
    serverItem.querySelector('.server-gamemode').textContent = info.serverGameDescription ?? '';

    const pingEl = serverItem.querySelector('.server-ping');

    if (ping) {
      pingEl.textContent = ping + 'ms';

      if (ping < 50) {
        pingEl.classList.add('text-green-500');
      } else if (ping < 100) {
        pingEl.classList.add('text-yellow-500');
      } else {
        pingEl.classList.add('text-red-500');
      }
    } else {
      pingEl.textContent = '';
    }
  }

  const serverItem = serverBrowserItemTemplate.content.firstElementChild.cloneNode(true);

  // convert the serverIp decimal to IP address
  if (!serverAddress) {
    const ip = info.serverIp;
    const ipA = (ip >> 24) & 0xFF;
    const ipB = (ip >> 16) & 0xFF;
    const ipC = (ip >> 8) & 0xFF;
    const ipD = ip & 0xFF;

    serverAddress = `${ipA}.${ipB}.${ipC}.${ipD}:${info.serverPort}`;

    // If this is sparse data, mark it as loading and update it when the full data is available
    serverItem.classList.add('text-gray-500', 'italic', 'opacity-70');
    serverItem.dataset.serverIp = info.serverIp;
    serverItem.dataset.serverPort = info.serverPort;
    serverItem.dataset.serverPortQuery = info.serverPortQuery;
  } else {
    // If this is full data, find the related sparse data and update it
    const sparseItems = serverListContainer.querySelectorAll(`tr[data-server-ip="${info.serverIp}"][data-server-port="${info.serverPort}"]`);

    sparseItems.forEach(sparseItem => {
      sparseItem.classList.remove('text-gray-500', 'italic', 'opacity-70');

      setServerItemData(sparseItem);
    });
  }

  setServerItemData(serverItem);

  if (info.serverTab === 'favorites') {
    serverItem.querySelector('.server-action-favorite').classList.add('hidden');
    serverItem.querySelector('.server-action-unfavorite').classList.remove('hidden');
  } else {
    serverItem.querySelector('.server-action-favorite').classList.remove('hidden');
    serverItem.querySelector('.server-action-unfavorite').classList.add('hidden');
  }

  serverItem.querySelector('.server-action-favorite')
    .addEventListener('click', () => {
      window.GameUI.ModifyFavoriteGame({
        appId: info.serverAppId,
        ip: info.serverIp,
        port: info.serverPort,
        portQuery: info.serverPortQuery,
      }, function (wasSuccessful) {
        // TODO: Refresh only the favorites tab
      })
    });

  serverItem.querySelector('.server-action-unfavorite')
    .addEventListener('click', () => {
      window.GameUI.ModifyFavoriteGame({
        appId: info.serverAppId,
        ip: info.serverIp,
        port: info.serverPort,
        portQuery: info.serverPortQuery,
        isRemoving: true,
      }, function (wasSuccessful) {
        if (wasSuccessful) {
          serverItem.remove();
        }
      })
    });

  // Add double-click event to server row for join
  serverItem.addEventListener('dblclick', function () {
    const serverName = info.serverHostName;
    const address = info.serverAddress;

    showLoadingOverlay('Connecting to server "' + serverName + '"...', function (finish) {
      window.GameUI.ConnectToServer({
        address: address,
      }, function (wasSuccessful) {
        finish();
      });
    });
  });

  serverListTable.appendChild(serverItem);
}

const pageElement = document.querySelector('#page');
const pageTitleElement = document.querySelector('#pageTitle');
const pageContentElement = document.querySelector('#pageContent');
const contentListElement = document.querySelector('#contentList');
const contentItemTemplateElement = document.querySelector('#contentItemTemplate');

const registeredPages = new Map();
let currentHideTimeout = null;
let currentPage = null;

customElements.define('game-menu-link', class extends HTMLElement {
  constructor() {
    super();

    this.addEventListener('click', this.onClick);

    this.classList.add('group', 'flex', 'gap-2', 'items-center', 'hover:scale-125', 'cursor-pointer', 'transition');

    if (this.hasAttribute('label-above')) {
      this.classList.add('flex-col-reverse');
    } else {
      this.classList.add('flex-col');
    }

    const key = this.getAttribute('translation');
    console.log(key);
  }

  connectedCallback() {
    const sizeClassImage = this.hasAttribute('small') ? 'w-12 h-12' : 'w-24 h-24';
    const sizeClassText = this.hasAttribute('small') ? 'text-sm' : 'text-md';
    this.innerHTML = `
            <img src="${this.getAttribute('icon')}"
                alt="${this.textContent}"
                class="${sizeClassImage}">
            <span class="scale-0 opacity-0 group-hover:opacity-100 group-hover:scale-100 transition-all ${sizeClassText}">${this.textContent}</span>
        `;
  }

  onClick(event) {
    event.preventDefault();

    const href = this.getAttribute('href');

    if (href) {
      if (currentPage) {
        currentPage.hide();
      }

      window.location.href = href;
      return;
    }

    const pageRef = this.getAttribute('pageref');

    if (pageRef) {
      const registeredPage = registeredPages.get(pageRef);

      if (registeredPage.isShowing()) {
        registeredPage.hide();
        return;
      }

      if (currentPage) {
        currentPage.hide();
      }

      // Small delay to ensure the hide animation has started
      setTimeout(() => {
        registeredPage.show();
      }, 50);

      return;
    }
  }
});

// Custom element for pages (invisible until selected)
customElements.define('game-page', class extends HTMLElement {
  _isShowing = false;
  _contentHolder = null;
  _initialized = false;
  _initializeFunction = null;

  constructor() {
    super();
    this.classList.add('hidden');

    // Create a content holder div for this page
    this._contentHolder = document.createElement('div');
    this._contentHolder.classList.add('page-content-holder', 'h-full', 'flex', 'flex-col');
  }

  connectedCallback() {
    registeredPages.set(this.id, this);

    // Move all initial children to the content holder
    while (this.firstChild) {
      this._contentHolder.appendChild(this.firstChild);
    }
  }

  // Set an initialization function for this page
  setInitFunction(initFn) {
    this._initializeFunction = initFn;
  }

  show() {
    // Clear any pending hide operations
    if (currentHideTimeout) {
      clearTimeout(currentHideTimeout);
      currentHideTimeout = null;
    }

    this._isShowing = true;
    currentPage = this;

    pageTitleElement.textContent = this.getAttribute('title');

    // Clear current content
    while (pageContentElement.firstChild) {
      pageContentElement.removeChild(pageContentElement.firstChild);
    }

    // Append the content holder to the page content
    pageContentElement.appendChild(this._contentHolder);

    // Initialize the page if needed
    if (!this._initialized && this._initializeFunction) {
      this._initializeFunction();
      this._initialized = true;
    }

    // Make the page visible
    pageElement.classList.remove('opacity-0');
  }

  isShowing() {
    return this._isShowing;
  }

  hide() {
    this._isShowing = false;

    // Hide the page visually
    pageElement.classList.add('opacity-0');

    // Set timeout to move content back after transition
    currentHideTimeout = setTimeout(() => {
      if (pageContentElement.contains(this._contentHolder)) {
        pageContentElement.removeChild(this._contentHolder);
      }
      currentHideTimeout = null;
    }, 500);
  }
});

/**
 * Loads the mountable content info from the game, populating the list
 * and setting up the event listeners for mounting and unmounting content.
 */
function initialize() {
  // Setup mountable content page
  GameUI.LoadMountableContentInfo(function (registeredMountableContent) {
    const contentPage = registeredPages.get('content');

    contentPage.setInitFunction(() => {
      const contentList = contentPage._contentHolder.querySelector('#contentList');
      const contentItemTemplateElement = document.querySelector('#contentItemTemplate');

      if (!contentList) return;

      // Clear existing content
      contentList.innerHTML = '';

      // sort it by name first
      registeredMountableContent.sort((a, b) => a.name.localeCompare(b.name));
      registeredMountableContent.forEach((content, index) => {
        const contentItemElement = contentItemTemplateElement.content.firstElementChild.cloneNode(true);

        contentItemElement.classList.add(index % 2 === 0 ? 'bg-white/10' : 'bg-white/5');

        const iconElement = contentItemElement.querySelector('img');
        iconElement.src = content.icon;
        iconElement.alt = content.name;

        const nameElement = contentItemElement.querySelector('span');
        nameElement.textContent = content.name;

        const inputElement = contentItemElement.querySelector('input');
        inputElement.value = content.id;
        inputElement.checked = content.mounted;

        inputElement.addEventListener('change', () => {
          if (inputElement.checked) {
            showLoadingOverlay('Mounting content...', function (finish) {
              GameUI.MountGameContent(inputElement.value, function (wasSuccessful) {
                if (!wasSuccessful) {
                  inputElement.checked = false;
                }

                finish();
              });
            });
          } else {
            showLoadingOverlay('Unmounting content...', function (finish) {
              GameUI.UnmountGameContent(inputElement.value, function (wasSuccessful) {
                if (!wasSuccessful) {
                  inputElement.checked = true;
                }

                finish();
              });
            });
          }
        });

        contentItemElement.addEventListener('click', (ev) => {
          // Don't toggle the checkbox if the click was on the checkbox itself
          // otherwise we immediately toggle it back
          if (ev.target.tagName === 'INPUT') {
            return;
          }

          inputElement.checked = !inputElement.checked;
          inputElement.dispatchEvent(new Event('change'));
        });

        contentList.appendChild(contentItemElement);
      });
    });
  });

  // Setup the host server page
  const hostServerPage = registeredPages.get('host-server');

  hostServerPage.setInitFunction(() => {
    // This function only runs when the host-server page is shown for the first time
    const pageContent = hostServerPage._contentHolder;
    const gameModeSelect = pageContent.querySelector('#gameMode');
    const mapSelect = pageContent.querySelector('#mapSelect');
    const mapPreview = pageContent.querySelector('#mapPreview');
    const startServerBtn = pageContent.querySelector('#startServerBtn');

    if (!gameModeSelect || !mapSelect) return;

    function populateGameModes(gamemodes) {
      gameModeSelect.innerHTML = '';

      gamemodes.forEach(gamemode => {
        const option = document.createElement('option');
        option.value = gamemode.id;
        option.textContent = gamemode.id;
        gameModeSelect.appendChild(option);
      });
    }

    function populateMaps(maps) {
      mapSelect.innerHTML = '';

      maps.forEach(map => {
        const option = document.createElement('option');
        option.value = map.id;
        option.textContent = map.id;
        map.preview = option.dataset.preview = 'images/maps/' + map.id + '.jpg';
        mapSelect.appendChild(option);
      });

      // Initialize with first map
      if (maps.length > 0) {
        updateMapPreview(maps[0]);
      }
    }

    GameUI.LoadServerVariables(function (availableMaps, availableGamemodes) {
      populateGameModes(availableGamemodes);
      populateMaps(availableMaps);
    });

    // Event listener for map selection
    mapSelect.addEventListener('change', function () {
      const selectedOption = this.options[this.selectedIndex];
      const map = {
        id: selectedOption.value,
        name: selectedOption.textContent,
        preview: selectedOption.dataset.preview
      };

      updateMapPreview(map);
    });

    function updateMapPreview(map) {
      if (map.preview) {
        mapPreview.src = map.preview;
      } else {
        mapPreview.src = 'images/maps/_placeholder.jpg';
      }
    }

    // Start server button
    startServerBtn.addEventListener('click', function () {
      const serverName = pageContent.querySelector('#serverName').value || "My Awesome Server";
      const gameMode = gameModeSelect.value;
      const map = mapSelect.value;
      const maxPlayers = pageContent.querySelector('#maxPlayers').value;
      const password = pageContent.querySelector('#serverPassword').value;

      showLoadingOverlay('Starting Server...', function (finish) {
        // Create server configuration object
        const serverConfig = {
          name: serverName,
          gamemode: gameMode,
          map: map,
          maxPlayers: parseInt(maxPlayers, 10),
          password: password
        };

        window.GameUI.HostServer(serverConfig, function (wasSuccessful) {
          finish();
        });
      });
    });
  });

  // Setup the server list page
  const serverBrowserPage = registeredPages.get('server-browser');

  serverBrowserPage.setInitFunction(() => {
    const refreshAction = pageContent.querySelector('#refreshAction');

    function refresh() {
      const serverListContainer = document.getElementById('server-list-container');
      const tableBodies = serverListContainer.querySelectorAll('table tbody');

      // Clear all table bodies
      tableBodies.forEach(body => {
        body.innerHTML = '';
      });

      window.GameUI.CancelServerListRequest();

      window.GameUI.RequestServerList(function (wasSuccessful) {
        // console.log('(Refresh) Server list requested', wasSuccessful);
      });
    }

    refresh();

    refreshAction.addEventListener('click', function () {
      refresh();
    });

    const tabButtons = document.querySelectorAll('.server-tab-button');
    const tabContents = document.querySelectorAll('.server-tab-content');
    let activeTab = 'internet';

    tabButtons.forEach(button => {
      button.addEventListener('click', function () {
        // Skip for legacy browser tab which uses the direct command
        if (this.id === 'tab-legacy') return;

        // Remove active class from all buttons
        tabButtons.forEach(btn => {
          btn.classList.remove('bg-primary-600');
          btn.classList.add('bg-white/10');
        });

        this.classList.remove('bg-white/10');
        this.classList.add('bg-primary-600');

        // Hide all tab contents
        tabContents.forEach(content => {
          content.classList.add('hidden');
        });

        // Show the corresponding tab content
        const tabId = this.id.replace('tab-', '');
        activeTab = tabId;
        document.getElementById('content-' + tabId).classList.remove('hidden');
      });
    });

    // Search input filter
    const searchInput = document.querySelector('input.server-search');
    const searchButton = searchInput.nextElementSibling;

    searchButton.addEventListener('click', function () {
      const searchTerm = searchInput.value.toLowerCase();
      const allServerRows = document.querySelectorAll('#content-' + activeTab + ' tbody tr');

      allServerRows.forEach(row => {
        const serverName = row.querySelector('.server-name')
          .textContent
          .toLowerCase();
        const gamemode = row.querySelector('.server-gamemode')
          .textContent
          .toLowerCase();

        if (serverName.includes(searchTerm) || gamemode.includes(searchTerm)) {
          row.style.display = '';
        } else {
          row.style.display = 'none';
        }
      });
    });

    // Filter by gamemode
    const searchFilter = document.querySelector('select.server-filter');
    searchFilter.addEventListener('change', function () {
      const selectedFilter = searchFilter.value.toLowerCase();
      const allServerRows = document.querySelectorAll('#content-' + activeTab + ' tbody tr');

      allServerRows.forEach(row => {
        const serverType = row.querySelector('.server-gamemode')
          .textContent
          .toLowerCase();

        if (selectedFilter === 'all' || serverType === selectedFilter) {
          row.style.display = '';
        } else {
          row.style.display = 'none';
        }
      });
    });
  });
}

window.addEventListener('interop:ready', () => {
  // Intro effect when the page is loaded
  document.body.classList.add('loaded');

  initialize();
});

// When working in the browser we want to mock the GameUI API
window.addEventListener('interop:installmock', () => {
  let refreshInterval;

  window.GameUI = {
    MountGameContent: function (value, callback) {
      console.log(`Mocking mounting content ${value}`);
      setTimeout(() => {
        callback(true);
      }, 1000);
    },

    UnmountGameContent: function (value, callback) {
      console.log(`Mocking unmounting content ${value}`);
      setTimeout(() => {
        callback(true);
      }, 1000);
    },

    LoadMountableContentInfo: function (callback) {
      callback([
        {
          id: 4000,
          name: 'Garry\'s Mod',
          mounted: true,
          icon: './images/game-icons/garrysmod.png',
        },
        {
          id: 240,
          name: 'Counter-Strike: Source',
          mounted: false,
          icon: './images/game-icons/cstrike.png',
        },
        {
          id: 220,
          name: 'Half-Life 2',
          mounted: false,
          icon: './images/game-icons/hl2.png',
        },
        {
          id: 280,
          name: 'Half-Life',
          mounted: true,
          icon: './images/game-icons/hl1.png',
        },
        {
          id: 300,
          name: 'Day of Defeat: Source',
          mounted: false,
          icon: './images/game-icons/dod.png',
        },
        {
          id: 340,
          name: 'Half-Life 2: Lost Coast',
          mounted: false,
          icon: './images/game-icons/lostcoast.png',
        },
        {
          id: 320,
          name: 'Half-Life Deathmatch: Source',
          mounted: false,
          icon: './images/game-icons/hl1mp.png',
        },
        {
          id: 380,
          name: 'Half-Life 2: Episode One',
          mounted: false,
          icon: './images/game-icons/episodic.png',
        },
        {
          id: 400,
          name: 'Portal',
          mounted: false,
          icon: './images/game-icons/portal.png',
        },
        {
          id: 420,
          name: 'Half-Life 2: Episode Two',
          mounted: false,
          icon: './images/game-icons/ep2.png',
        },
        {
          id: 440,
          name: 'Team Fortress 2',
          mounted: false,
          icon: './images/game-icons/tf.png',
        },
      ]);
    },

    LoadServerVariables: function (callback) {
      const mockMaps = [
        { id: 'gm_flatgrass' },
        { id: 'gm_construct' },
        { id: 'cs_office' },
        { id: 'de_dust2' },
        { id: 'c17_hospital' }
      ];

      const mockGamemodes = [
        { id: 'sandbox' },
        { id: 'deathmatch' },
        { id: 'teamdm' },
        { id: 'rp' },
        { id: 'zombies' }
      ];

      console.log(`Loading server variables (mock)`);
      setTimeout(() => {
        callback(mockMaps, mockGamemodes);
      }, 500);
    },

    HostServer: function (config, callback) {
      console.log(`Mocking host server with config:`, config);
      setTimeout(() => {
        callback(true);
      }, 2000);
    },

    ModifyFavoriteGame: function (serverFavoriteInfo, callback) {
      console.log(`Mocking modify favorite game:`, serverFavoriteInfo);
      setTimeout(() => {
        callback(true);
      }, 1000);
    },

    ConnectToServer: function (serverInfo, callback) {
      console.log(`Mocking connect to server:`, serverInfo);
      setTimeout(() => {
        callback(true);
      }, 1000);
    },

    RequestServerList: function (callback) {
      callback(true);

      // Start calling the server list functions with mock data
      const mockServerInfo = [
        {
          serverAddress: '192.168.123.1:27015',
          serverIp: 3232267009,
          serverPort: 27015,
          serverPortQuery: 27001,
          serverPing: 25,
          serverHostName: '[US-East] Official Experiment Server',
          serverGameDirectory: 'experiment',
          serverMap: 'exp_cascade_falls',
          serverGameDescription: 'Team Deathmatch',
          serverAppId: 243750,
          serverPlayers: 24,
          serverMaxPlayers: 32,
          serverBotPlayers: 0,
          serverHasPassword: false,
          serverIsSecure: true,
          serverTimeLastPlayed: 1631870400,
          serverVersion: 1,
          serverGameTags: 'teamdeathmatch,official',
          serverSteamId: '76561198000000000',
          serverTab: 'internet',
        },
        {
          serverAddress: '192.168.123.1:27015',
          serverIp: 3232267009,
          serverPort: 27015,
          serverPortQuery: 27001,
          serverPing: 50,
          serverHostName: 'Experiment Pro League #1',
          serverGameDirectory: 'experiment',
          serverMap: 'exp_orbital_station',
          serverGameDescription: 'Capture the Flag',
          serverAppId: 243750,
          serverPlayers: 12,
          serverMaxPlayers: 16,
          serverBotPlayers: 0,
          serverHasPassword: false,
          serverIsSecure: true,
          serverTimeLastPlayed: 1631870400,
          serverVersion: 1,
          serverGameTags: 'ctf,proleague',
          serverSteamId: '76561198000000001',
          serverTab: 'internet',
        },
        {
          serverAddress: '192.168.123.1:27015',
          serverIp: 3232267009,
          serverPort: 27015,
          serverPortQuery: 27001,
          serverPing: 75,
          serverHostName: '[EU] DevTest Server',
          serverGameDirectory: 'experiment',
          serverMap: 'exp_metropolis',
          serverGameDescription: 'Control Point',
          serverAppId: 243750,
          serverPlayers: 8,
          serverMaxPlayers: 16,
          serverBotPlayers: 0,
          serverHasPassword: false,
          serverIsSecure: true,
          serverTimeLastPlayed: 1631870400,
          serverVersion: 1,
          serverGameTags: 'controlpoint,devtest',
          serverSteamId: '76561198000000002',
          serverTab: 'internet',
        },
        /*
          History and favorites information starts sparse, but will be filled
          in once the server list request is complete.
        */
        {
          serverAppId: 243750,
          serverIp: 3229318011, // 192.123.123.123
          serverPort: 27015,
          serverPortQuery: 27001,
          serverTab: 'favorites',
        },
        {
          serverAppId: 243750,
          serverIp: 3242345025, // 192.321.321.321
          serverPort: 27015,
          serverIpQuery: 3242345025,
          serverPortQuery: 27001,
          serverTab: 'history',
        },
        /*
          Mock data to fill in the rest of the favorites and history, which
          is matched by the serverIp and serverPort.
         */
        {
          serverAddress: '192.123.123.123:27015',
          serverIp: 3229318011,
          serverPort: 27015,
          serverPortQuery: 27001,
          serverPing: 100,
          serverHostName: 'My Favorite Server',
          serverGameDirectory: 'experiment',
          serverMap: 'exp_cascade_falls',
          serverGameDescription: 'Team Deathmatch',
          serverPlayers: 16,
          serverMaxPlayers: 24,
          serverBotPlayers: 0,
          serverHasPassword: false,
          serverIsSecure: true,
          serverTimeLastPlayed: 1631870400,
          serverVersion: 1,
          serverGameTags: 'teamdeathmatch,favorite',
          serverSteamId: '76561198000000003',
          serverTab: 'internet',
        },
        {
          serverAddress: '192.321.321.321:27015',
          serverIp: 3242345025,
          serverPort: 27015,
          serverPortQuery: 27001,
          serverPing: 150,
          serverHostName: 'My History Server',
          serverGameDirectory: 'experiment',
          serverMap: 'exp_cascade_falls',
          serverGameDescription: 'Team Deathmatch',
          serverPlayers: 16,
          serverMaxPlayers: 24,
          serverBotPlayers: 0,
          serverHasPassword: false,
          serverIsSecure: true,
          serverTimeLastPlayed: 1631870400,
          serverVersion: 1,
          serverGameTags: 'teamdeathmatch,history',
          serverSteamId: '76561198000000004',
          serverTab: 'internet',
        },
      ];

      let index = 0;

      refreshInterval = setInterval(() => {
        if (index >= mockServerInfo.length) {
          clearInterval(refreshInterval);
          return;
        }

        const serverInfo = mockServerInfo[index];

        window.ServerListAdd(serverInfo);
        index++;
      }, 300);
    },

    CancelServerListRequest: function () {
      clearInterval(refreshInterval);
    },
  };
});

<style>
  .tab-pane { counter-reset: place; }
  .race-result [data-note]:empty { display: none; }
  .race-result {
    opacity: 1;
    animation: fadeUp 1s forwards;
    will-change: opacity, transform;
  }

  .prelims .race-result,
  .finals .race-result {
    background-color: #b6d7a8 !important;
  }

  .prelims .race-result:nth-child(n+11),
  .finals .race-result:nth-child(n+7) {
    background-color: #efefef !important;
  }

  .race-result [data-place]::before {
    counter-increment: place;
    content: counter(place);
    position: absolute;
  }
  .race-result.dq.dq {
    background-color: #fff !important;
  }
  .race-result.dq [data-place]::before {
    content: 'DQ';
  }

  @keyframes fadeUp {
    0% {
      opacity: 0;
      transform: translateY(50px);
    }
    30% {
      opacity: 0;
    }
    100% {
      opacity: 1;
      transform: translateY(0);
    }
  }
</style>

<div id="results">
  <ul class="nav nav-tabs mb-2" role="tablist">
    <li class="nav-item">
      <a id="womens-prelims-tab" href="#tab-womens-prelims" class="nav-link" data-toggle="tab" role="tab" aria-controls="tab-womens-prelims" aria-selected="true">Women's Prelims</a>
    </li>
    <li class="nav-item">
      <a id="mens-prelims-tab" href="#tab-mens-prelims" class="nav-link" data-toggle="tab" role="tab" aria-controls="tab-mens-prelims" aria-selected="false">Men's Prelims</a>
    </li>
    <li class="nav-item">
      <a id="allgender-prelims-tab" href="#tab-allgender-prelims" style="display: none" class="nav-link" data-toggle="tab" role="tab" aria-controls="tab-allgender-prelims" aria-selected="false">All Gender Prelims</a>
    </li>
    <li class="nav-item">
      <a id="womens-finals-tab" href="#tab-womens-finals" style="display: none" class="nav-link" data-toggle="tab" role="tab" aria-controls="tab-womens-finals" aria-selected="false">Women's Finals</a>
    </li>
    <li class="nav-item">
      <a id="mens-finals-tab" href="#tab-mens-finals" style="display: none" class="nav-link" data-toggle="tab" role="tab" aria-controls="tab-mens-finals" aria-selected="false">Men's Finals</span></a>
    </li>
    <li class="nav-item">
      <a id="allgender-finals-tab" href="#tab-allgender-finals" style="display: none" class="nav-link" data-toggle="tab" role="tab" aria-controls="tab-allgender-finals" aria-selected="false">All Gender Finals</a>
    </li>
    <li class="nav-item">
      <a id="exhibitions-tab" href="#tab-exhibitions" style="display: none" class="nav-link active" data-toggle="tab" role="tab" aria-controls="tab-exhibitions" aria-selected="false">Exhibitions</a>
    </li>
  </ul>

  <p>Results update automatically.</p>

  <div class="tab-content mb-3">
    <div class="tab-pane fade prelims" id="tab-womens-prelims" role="tabpanel" aria-labelledby="womens-prelims-tab">
    </div>
    <div class="tab-pane fade prelims" id="tab-mens-prelims" role="tabpanel" aria-labelledby="mens-prelims-tab">
    </div>
    <div class="tab-pane fade prelims" id="tab-allgender-prelims" role="tabpanel" aria-labelledby="allgender-prelims-tab">
    </div>
    <div class="tab-pane fade finals" id="tab-womens-finals" role="tabpanel" aria-labelledby="womens-finals-tab">
    </div>
    <div class="tab-pane fade finals" id="tab-mens-finals" role="tabpanel" aria-labelledby="mens-finals-tab">
    </div>
    <div class="tab-pane fade prelims" id="tab-allgender-finals" role="tabpanel" aria-labelledby="allgender-finals-tab">
    </div>
    <div class="tab-pane fade prelims show active" id="tab-exhibitions" role="tabpanel" aria-labelledby="exhibitions-tab">
    </div>
  </div>
</div>

<template id="raceResult">
  <div class="race-result p-3 mb-1 border rounded" data-id>
    <div class="d-flex justify-content-between" data-place>
      <div data-name class="ml-5"></div>
      <div data-time class="ml-2 text-monospace"></div>
    </div>
    <div class="d-flex justify-content-between">
      <div data-note class="ml-5 mt-1 text-dark"></div>
      <em><div data-speed class="ml-2"></div></em>
    </div>
  </div>
</template>

<script src="https://cdn.polyfill.io/v2/polyfill.min.js"></script>
<script src="https://www.gstatic.com/firebasejs/4.12.1/firebase.js"></script>
<script>
  // Initialize Firebase
  var config = {
    apiKey: 'AIzaSyCjdOwIz6r5P0OScBb84x9o9udtveSKpFM',
    authDomain: 'cmu-buggy-leaderboard.firebaseapp.com',
    databaseURL: 'https://cmu-buggy-leaderboard.firebaseio.com',
    projectId: 'cmu-buggy-leaderboard',
    storageBucket: 'cmu-buggy-leaderboard.appspot.com',
    messagingSenderId: '31785879580'
  };
  firebase.initializeApp(config);

  // Determine which tabs are visible.
  var visibleconfigref = firebase.database().ref("config").orderByChild('time_sort');
  visibleconfigref.on('value', function (snapshot) {
    snapshot.forEach(function (child) {
      if (child.key == 'CURRENT_TAB') {
        // not currently relevant in this view
        return;
      }
      var nameStyle = child.key.replace('_', '-');
      var button = document.querySelector('#' + nameStyle + '-tab');

      var displaySetting = child.val() == true ? 'block' : 'none';
      button.style.display = displaySetting;

      console.debug("%s set to %s", nameStyle, displaySetting);
    })
  });

  var raceResult = document.querySelector('#raceResult');

  function populateNodeText(node, value) {
    node.querySelector('[data-name]').textContent = value.team;
    node.querySelector('[data-time]').textContent = value.time || ' ';
    node.querySelector('[data-note]').textContent = value.notes || ' ';
    node.querySelector('[data-speed]').textContent =
      (value.speed_trap ? value.speed_trap + ' mph' : ' ');
  }

  function addRaceResult(tab, data, previousChildKey) {
    var node = document.importNode(raceResult.content, true),
      value = data.val(),
      previousNode = previousChildKey && tab.querySelector('[data-id=' + previousChildKey + ']');
    node.querySelector('[data-id]').setAttribute('data-id', data.key);

    populateNodeText(node, value);

    if (value.dq) {
      node.querySelector('.race-result').classList.add('dq');
    }
    if (previousNode) {
      tab.insertBefore(node, previousNode.nextSibling);
    } else {
      tab.insertBefore(node, tab.firstChild);
    }
    console.debug('result added: %s - %s', tab.id, data.key);
  }

  function updateRaceResult(tab, data, previousChildKey) {
    var node = tab.querySelector('[data-id=' + data.key + ']'),
    value = data.val();

    populateNodeText(node, value);

    if (value.dq) {
      node.classList.add('dq');
    } else {
      node.classList.remove('dq');
    }
    console.debug('result updated: %s - %s', tab.id, data.key);
  }

  function removeRaceResult(tab, data) {
    tab.removeChild(tab.querySelector('[data-id=' + data.key + ']'))
    console.debug('result removed: %s - %s', tab.id, data.key);
  }

  function moveRaceResult(tab, data, previousChildKey) {
    var node = tab.querySelector('[data-id=' + data.key + ']'),
      previousNode = previousChildKey && tab.querySelector('[data-id=' + previousChildKey + ']');
    tab.removeChild(node);
    if (previousNode) {
      tab.insertBefore(node, previousNode.nextSibling);
    } else {
      tab.insertBefore(node, tab.firstChild);
    }
    console.debug('result moved: %s - %s', tab.id, data.key);
  }

  ['womens_prelims', 'mens_prelims', 'allgender_prelims',
   'womens_finals', 'mens_finals', 'allgender_finals',
   'exhibitions'].forEach(function (type) {
    var ref = firebase.database().ref(type).orderByChild('time_sort'),
      tab = document.querySelector('#tab-' + type.replace('_', '-'));
    ref.on('child_added', function (snapshot, previousChildKey) {
      addRaceResult(tab, snapshot, previousChildKey);
    });
    ref.on('child_removed', function (snapshot) {
      removeRaceResult(tab, snapshot);
    });
    ref.on('child_changed', function (snapshot, previousChildKey) {
      updateRaceResult(tab, snapshot, previousChildKey);
    });
    ref.on('child_moved', function (snapshot, previousChildKey) {
      moveRaceResult(tab, snapshot, previousChildKey);
    });
  });

  // Escape hatch.  Add the following next to "Results update automatically." above
  //  If they stop or you can't see them, <a role="button" href="#" data-iframe>click here</a>.
  // document.querySelector('[data-iframe]').addEventListener('click', function (e) {
  //  document.querySelector('#results').innerHTML = '<iframe src="https://docs.google.com/spreadsheets/d/e/2PACX-1vQRPv3lcYc3ZAM4rAlXvtRaJIJXUdtm4EKjk3rS22rpGQ-awGmdxqsSX2dSTJsWzN7uJT1v9ax5qwJi/pubhtml?widget=true&amp;headers=false" style="width: 100%; height: 80vh;"></iframe>';
  // })
</script>

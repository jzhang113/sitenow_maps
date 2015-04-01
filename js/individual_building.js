(function($) {
	Drupal.individualBuilding = function() {
    var map;
    var MY_MAPTYPE_ID = 'custom_style';
    var featureOpts = [
        {
          "stylers": [
            { "visibility": "off" },
          ]
        },
        {
          "featureType": "road",
          "stylers": [
            { "visibility": "on" }
          ]
        },
        {
          "featureType": "landscape",
          "stylers": [
            { "visibility": "on" }
          ]
        },
        {
          "featureType": "water",
          "stylers": [
            { "visibility": "on" }
          ]
        },
        {
          "featureType": "landscape.man_made",
          "stylers": [
            { "visibility": "off" }
          ]
        }
      ];

      var mapOptions = {
        zoom: 15,
        center: new google.maps.LatLng(41.660070, -91.538403),
        mapTypeId: MY_MAPTYPE_ID,
        disableDefaultUI: true
      };
      map = new google.maps.Map(document.getElementById('building_map_'+Drupal.settings.abbr), mapOptions);

      var styledMapOptions = {
        name: 'Custom Style'
      };

      var customMapType = new google.maps.StyledMapType(featureOpts, styledMapOptions);

      map.mapTypes.set(MY_MAPTYPE_ID, customMapType);



    jQuery.get("http://data.its.uiowa.edu/maps/arc-buildings",
          function(data){
            map.setCenter(google.maps.LatLng(Drupal.settings.latitude[0],Drupal.settings.longitude[0]));
          });

	}

	Drupal.behaviors.individualBuilding = {
    attach: function(context, settings) {
      $('.building_panel_'+settings.abbr, context).once('individualBuilding', function() {
        Drupal.individualBuilding();
      });
    }
  };
})(jQuery);



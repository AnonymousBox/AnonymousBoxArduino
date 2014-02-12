angular.module('Anonymousbox', ['ngRoute'])
    .config(['$routeProvider', function($routeProvider) {
                $routeProvider.
                    when('/', { templateUrl: 'partials/messages.jade', controller: MessagesCtrl}).
                                otherwise({ redirectTo: '/' });
    }]);

function MessagesCtrl($scope, $http){
    $http.get('/getmessages').
        success(function(data,status,headers,config){
            console.log("got messages: ", data);
            $scope.messages = data;
        });
}

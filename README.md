# IoT Environmental Monitoring Station

Tested with Arduino Libraries:

ThingsBoard 0.6.0
PubSubClient Nick O'Leary 2.8.0 
ArduinoJson Benoit Blanchon 6.19.3
ArduinoHTTPClient 0.4.0

# Extract telemetry data from Thingsboard API

https://thingsboard-server:8080/swagger-ui/

Get time-series data (getTimeseries)

/api/plugins/telemetry/{entityType}/{entityId}/values/timeseries{?agg,endTs,interval,keys,limit,orderBy,startTs,useStrictDataTypes}

Click on the lock and authenticate as tenant (not as administrator)

Example:

entityType DEVICE
entityId 7d904af0-29f8-11ed-8bc5-f192f58a57d5
startTs 1677711600000
endTs 1677884400000
interval 60000

Execute

# Convert JSON telemetry data to CSV

apt install jq

jq -r '.temperature[] | [.ts, .value] | @csv' response_1680598626461.json > response.csv



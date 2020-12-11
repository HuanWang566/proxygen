import urllib.parse
import httplib2


url_root = 'http://localhost:11000/'
http = httplib2.Http()


def get_arm_status(arm_id):
    url = url_root + 'get_arm_status?arm_id=' + str(arm_id)
    print(url)
    response, content = http.request(url, 'GET')
    return content.decode("utf-8").replace('encoding=\"gb2312\"', 'encoding=\"utf-8\"')


def get_car_pos(arm_id, car_id):
    url = url_root + 'get_car_status?car_id=' + str(car_id)
    print(url)
    response, content = http.request(url, 'GET')
    return content.decode("utf-8").replace('encoding=\"gb2312\"', 'encoding=\"utf-8\"')


def get_conveyor_status(conveyor_id=1):
    url = url_root + 'get_conveyor_status?conveyor_id=' + str(conveyor_id)
    print(url)
    response, content = http.request(url, 'GET')
    return content.decode("utf-8").replace('encoding=\"gb2312\"', 'encoding=\"utf-8\"')


def get_color_status(arm_id):
    url = url_root + 'get_block_color?arm_id=' + str(arm_id)
    print(url)
    response, content = http.request(url, 'GET')
    return content.decode("utf-8").replace('encoding=\"gb2312\"', 'encoding=\"utf-8\"')


def set_arm_status(arm_id, status):
    while True:
        url = url_root + 'set_arm_status?arm_id=' + \
            str(arm_id) + '&status=' + status
        print(url)
        response, content = http.request(url, 'GET')
        if content.decode("utf-8").replace('encoding=\"gb2312\"', 'encoding=\"utf-8\"') == "SUCCESS":
            return "SUCCESS"


def set_car_position(car_id, pos):
    while True:
        url = url_root + 'set_car_status?car_id=' + \
            str(car_id) + '&status=' + pos
        print(url)
        response, content = http.request(url, 'GET')
        if content.decode("utf-8").replace('encoding=\"gb2312\"', 'encoding=\"utf-8\"') == "SUCCESS":
            return "SUCCESS"


def set_conveyor_status(status, conveyor_id=1):
    while True:
        url = url_root + 'set_conveyor_status?conveyor_id=' + \
            str(conveyor_id) + '&status=' + status
        print(url)
        response, content = http.request(url, 'GET')
        if content.decode("utf-8").replace('encoding=\"gb2312\"', 'encoding=\"utf-8\"') == "SUCCESS":
            return "SUCCESS"


def set_color_status(arm_id, color):
    while True:
        url = url_root + 'set_block_color?arm_id=' + \
            str(arm_id) + '&color=' + color
        print(url)
        response, content = http.request(url, 'GET')
        if content.decode("utf-8").replace('encoding=\"gb2312\"', 'encoding=\"utf-8\"') == "SUCCESS":
            return "SUCCESS"


def get_set_arm_status_not(arm_id, not_status, status):
    url = url_root + 'get_set_arm_not_status?arm_id=' + \
        str(arm_id) + '&status=' + status + "&not_status=" + not_status
    print(url)
    response, content = http.request(url, 'GET')
    return content.decode("utf-8").replace('encoding=\"gb2312\"', 'encoding=\"utf-8\"') == "SUCCESS"


def get_set_arm_status_is(arm_id, is_status, status):
    url = url_root + 'get_set_arm_is_status?arm_id=' + str(arm_id) + \
        '&status=' + status + "&is_status=" + is_status
    print(url)
    response, content = http.request(url, 'GET')
    return content.decode("utf-8").replace('encoding=\"gb2312\"', 'encoding=\"utf-8\"') == "SUCCESS"


if __name__ == '__main__':
    arm_id = 3
    car_id = 2
    conveyor_id = 1

    print(get_arm_status(arm_id))
    print(get_car_pos(arm_id, car_id))
    print(get_conveyor_status(conveyor_id))
    print(get_color_status(arm_id))

    status_arm = "ready"
    status_car = "arm2"
    status_conveyor = "run"
    status_color = "red"

    print(set_arm_status(arm_id, status_arm))
    print(set_car_position(car_id, status_car))
    print(set_conveyor_status(status_conveyor, conveyor_id))
    print(set_color_status(arm_id, status_color))

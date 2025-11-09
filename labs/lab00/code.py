class Swallow:
    def __init__(self, swallow_type):
        self.type = swallow_type

def get_airspeed_velocity_of(unladen_swallow):
    if unladen_swallow.type == "african":
        return 22  # 假设值，单位 km/h
    elif unladen_swallow.type == "european":
        return 20  # 假设值，单位 km/h
    else:
        raise ValueError("Unknown swallow type")

def fizzbuzz(num):
    if num % 3 == 0 and num % 5 == 0:
        print(f"{num}: fizzbuzz")
    elif num % 3 == 0:
        print(f"{num}: fizz")
    elif num % 5 == 0:
        print(f"{num}: buzz")

for i in range(1, 20):
    fizzbuzz(i)

# 测试燕子速度函数
african = Swallow("african")
european = Swallow("european")
print(f"African swallow speed: {get_airspeed_velocity_of(african)} km/h")
print(f"European swallow speed: {get_airspeed_velocity_of(european)} km/h")

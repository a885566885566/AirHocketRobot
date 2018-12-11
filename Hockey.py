import numpy as np

np.seterr(all="ignore")


class Hockey:

    def __init__(self, corners, ball_pos):
        self.corners = np.array(corners)
        self.ball_pos = np.array(ball_pos)
        self.ball_vec = np.array([0, 0])

    def move(self, ball_pos):
        move_vec = ball_pos - self.ball_pos
        # self.ball_vec = (self.ball_vec + move_vec)*0.5
        self.ball_vec = move_vec
        self.ball_pos = ball_pos

    def cal_collide(self):

        """function return collide point(x,y) or None if calculation fails"""
        """若球極靠近邊界 結果可能會受到點數誤差影響"""

        result = []
        vec_wall_b = []

        for i in range(4):  # 對四個邊做交點計算並判斷是否為可碰撞點
            a = self.corners[0]
            b = self.corners[1]

            vec_wall = np.roll(b - a, 1)
            vec_wall[1] *= -1

            vec_ball = np.roll(self.ball_vec, 1)
            vec_ball[1] *= -1

            aug_mat = np.mat([np.hstack((vec_wall, np.dot(vec_wall, a))),
                              np.hstack((vec_ball, np.dot(vec_ball, self.ball_pos)))])

            D = np.linalg.det(aug_mat[:, :2])
            Dx = np.linalg.det(aug_mat[:, 2:0:-1])
            Dy = np.linalg.det(aug_mat[:, 0::2])

            col_point = np.around((Dx/D, Dy/D), 2)

            if (a[0] <= col_point[0] <= b[0] or b[0] <= col_point[0] <= a[0]) \
                    and (a[1] <= col_point[1] <= b[1] or b[1] <= col_point[1] <= a[1]):  # 落點範圍判斷
                if self.ball_vec[0] != 0 and ((col_point[0]-self.ball_pos[0])/self.ball_vec[0] >= 0):  # 落點方向判斷
                    result.append(col_point)
                    vec_wall_b = vec_wall
                    break
                elif self.ball_vec[1] != 0 and ((col_point[1]-self.ball_pos[1])/self.ball_vec[1] >= 0):
                    result.append(col_point)
                    vec_wall_b = vec_wall
                    break

            self.corners = np.roll(self.corners, 1, axis=0)

        # 計算牆面角度及反彈方向向量
        try:
            theta2 = 2*np.arctan2(vec_wall_b[1], vec_wall_b[0])
        except IndexError:
            print("ball's not moving")
            return result
        boun_vec = [self.ball_vec.dot((np.cos(theta2), np.sin(theta2))),
                    self.ball_vec.dot((np.sin(theta2), -np.cos(theta2)))]

        # 計算第二個碰撞點
        for i in range(4):
            self.corners = np.roll(self.corners, 1, axis=0)
            a = self.corners[0]
            b = self.corners[1]

            vec_wall = np.roll(b - a, 1)
            vec_wall[1] *= -1

            vec_boun = np.roll(boun_vec, 1)
            vec_boun[1] *= -1

            aug_mat = np.mat([np.hstack((vec_wall, np.dot(vec_wall, a))),
                              np.hstack((vec_boun, np.dot(vec_boun, result[0])))])

            D = np.linalg.det(aug_mat[:, :2])
            Dx = np.linalg.det(aug_mat[:, 2:0:-1])
            Dy = np.linalg.det(aug_mat[:, 0::2])
            # print("D={}, Dx={}, Dy={}".format(D, Dx, Dy))
            col_point = np.around((Dx / D, Dy / D), 2)

            if (a[0] <= col_point[0] <= b[0] or b[0] <= col_point[0] <= a[0]) \
                    and (a[1] <= col_point[1] <= b[1] or b[1] <= col_point[1] <= a[1]):  # 落點範圍判斷
                result.append(col_point)
                break
        return result

"""
point_a = np.array([1, 2])
point_b = np.array([4, 5])
vec_ab = point_b - point_a
print(vec_ab)
vec_ab[1]*=-1
print(vec_ab)
line_ab = np.append(vec_ab, np.dot(vec_ab, point_a))
print(line_ab)

mat_a = np.mat([[3, 5], [1, 2]])
mat_b = np.mat([[4], [1]])
print(mat_a.I*mat_b)
"""


"""
mat_l = np.mat([vec_wall, vec_ball])
mat_r = np.mat([[np.dot(a, vec_wall)], [np.dot(self.ball_pos, vec_ball)]])

try:
    col_point = mat_l.I * mat_r
except np.linalg.linalg.LinAlgError:
    raise 
    print("ball's not moving, or maybe bug happening")
    break
"""
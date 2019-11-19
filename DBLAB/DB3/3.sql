-- 23
SELECT 教师姓名, 性别
FROM teacher
WHERE 教师ID号 IN (
SELECT 教师ID号 FROM teacher
WHERE 职称或职业 = '副教授' AND 性别 = '女'
)
-- 24
SELECT *
FROM teacher
WHERE 教师ID号 IN (
SELECT 教师编号 
FROM teacher_class_course
WHERE 课程号 = 'dep01_s002'
)
-- 25
SELECT 学生ID号, 学生姓名
FROM student
WHERE 学生ID号 IN (
SELECT 学生ID号
FROM student_course
WHERE 课程ID号 = 'dep04_s002'
)
-- 26
SELECT 学生ID号, 学生姓名
FROM student
WHERE 学生ID号 IN (
SELECT 学生ID号
FROM student_course
WHERE 分数 < 60
)
-- 27
SELECT 学生ID号, 学生姓名,性别,家庭住址
FROM student
WHERE 学生ID号 IN (
SELECT 学生ID号
FROM student_course
INNER JOIN course
ON course.课程号 = student_course.课程ID号
WHERE 分数 < 60 AND 课程名称 LIKE '%网页设计%'
)
-- 28
SELECT 学生ID号, 学生姓名
FROM student
WHERE 学生ID号 IN (
SELECT 学生ID号
FROM student_course
INNER JOIN course
ON course.课程号 = student_course.课程ID号
WHERE 课程名称 LIKE '%计算机基础%'
)
-- 29
SELECT 学生ID号, 学生姓名
FROM student
WHERE 学生ID号 NOT IN (
SELECT 学生ID号
FROM student_course
INNER JOIN course
ON course.课程号 = student_course.课程ID号
WHERE 课程名称 LIKE '%计算机基础%'
)
-- 30
SELECT S.学生ID号, S.学生姓名
FROM student S
WHERE not exists(
(SELECT T.课程ID号
FROM student_course T
WHERE T.学生ID号='g0940201'
)
EXCEPT
(SELECT 课程ID号
FROM student_course T
WHERE S.学生ID号=T.学生ID号)
)
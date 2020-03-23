-- 创建INSERT触发器，如果向教师表中插入记录时，检查该记录的院系编号在表中是否存在。如果不存在，则不允许插入
CREATE TRIGGER [dbo].[check_teacher] ON [dbo].[teacher]
FOR INSERT
AS
BEGIN
    DECLARE @id NCHAR(6);
    SELECT @id=部门ID号
    FROM INSERTED;
    IF (NOT EXISTS (SELECT *
    FROM [dbo].[department]
    WHERE 部门ID号 = @id))
    BEGIN
        ROLLBACK;
        PRINT '部门 ID 不存在';
        RETURN;
    END
END

-- TEST
INSERT INTO teacher
    (教师ID号,教师姓名,性别,出生时间, 部门ID号,职称或职业,联系电话,家庭地址,邮政编码,类别编号)
VALUES
    ('dep05_114', '田所浩二', '男', '1989-06-04', 'dep_10', '先辈', '1145141919', '', '', 5);

-- 创建UPDATEClass触发器，当对班级信息表主键"班级号"进行修改时，应对学生表中相应的"班级号"也进行修改
GO
CREATE TRIGGER UPDATEClass
ON [dbo].[class]
FOR UPDATE
AS
IF UPDATE(班级号)
BEGIN
    DECLARE @old NCHAR(6);
    SELECT @old=班级号
    FROM DELETED;
    DECLARE @new NCHAR(6);
    SELECT @new=班级号
    FROM INSERTED;
    UPDATE [dbo].[student]
    SET 班级ID号 =  @new
    FROM [dbo].[student]
    WHERE 班级ID号 = @old;
END

-- TEST
UPDATE [dbo].[class] SET 班级号='g11451' FROM [dbo].[class] WHERE 班级号='g09402'
UPDATE [dbo].[class] SET 班级号='g09402' FROM [dbo].[class] WHERE 班级号='g11451'

-- 定义触发器，向学生表插入一条记录时，将所有学生的学号值加1，并对其进行测试。
-- BROKEN BECAUSE FUCKING ID IS FUCKING NCHAR NOT FUCKING INT
GO
CREATE TRIGGER [dbo].[updatestu] ON [dbo].[student]
FOR INSERT
AS
BEGIN
    UPDATE [dbo].[student]
    SET 学生ID号 =  'g' + FORMAT((RIGHT(学生ID号,7) + 1), d7)
END

-- TESt 
INSERT INTO student
VALUES
    ('g1111111', '带带大师兄', 'g11111', '男', '1999-02-24', '1999-02-24', '')
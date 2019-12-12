import pyodbc
import prettytable


def print_table(conn, head=""):
    cursor = conn.cursor()
    teacher = cursor.execute("SELECT * FROM teacher")
    table = prettytable.from_db_cursor(teacher)
    print('%s teacher 表格数据' % head)
    print(table)


def delete_table(conn):
    cursor = conn.cursor()
    delete_sql = "DELETE FROM teacher WHERE [教师姓名]='田所浩二'"
    cursor.execute(delete_sql)
    conn.commit()


def insert_table(conn):
    cursor = conn.cursor()
    insert_sql = """
    INSERT INTO teacher
    (教师ID号,教师姓名,性别,出生时间, 部门ID号,职称或职业,联系电话,家庭地址,邮政编码,类别编号)
VALUES
    ('dep05_114', '田所浩二', '男', '1989-06-04', 'dep_10', '先辈', '1145141919', '', '', 5)
    """
    cursor.execute(insert_sql)
    conn.commit()


def update_table(conn):
    cursor = conn.cursor()
    update_sql = """
    UPDATE teacher
    SET 邮政编码='77777'
WHERE
    教师姓名='田所浩二'
    """
    cursor.execute(update_sql)
    conn.commit()


conn = pyodbc.connect(
    'DRIVER={SQL Server};SERVER=localhost;DATABASE=学生选课;UID=sa;PWD=114514')
print_table(conn)
insert_table(conn)
print_table(conn, head='插入田所先生后的')
update_table(conn)
print_table(conn, head='修改田所先生的邮编为 77777 后的')
delete_table(conn)
print_table(conn, head='删除田所先生后的')

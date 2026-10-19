-- 分布式聊天系统数据库初始化脚本
-- 创建数据库
CREATE DATABASE IF NOT EXISTS chat_db DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE chat_db;

-- 用户表
CREATE TABLE IF NOT EXISTS users (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(64) NOT NULL UNIQUE COMMENT '用户名',
    password VARCHAR(128) NOT NULL COMMENT '密码哈希',
    salt VARCHAR(64) NOT NULL COMMENT '密码盐',
    nickname VARCHAR(64) NOT NULL DEFAULT '' COMMENT '昵称',
    avatar VARCHAR(256) NOT NULL DEFAULT '' COMMENT '头像URL',
    status TINYINT NOT NULL DEFAULT 1 COMMENT '状态: 0=禁用, 1=正常',
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_username (username)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='用户表';

-- 消息表
CREATE TABLE IF NOT EXISTS messages (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    from_user_id BIGINT UNSIGNED NOT NULL COMMENT '发送者ID',
    to_user_id BIGINT UNSIGNED NOT NULL COMMENT '接收者ID',
    content TEXT NOT NULL COMMENT '消息内容',
    msg_type TINYINT NOT NULL DEFAULT 0 COMMENT '消息类型: 0=文本, 1=图片, 2=语音, 3=视频',
    is_read TINYINT NOT NULL DEFAULT 0 COMMENT '是否已读: 0=未读, 1=已读',
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_from_to (from_user_id, to_user_id),
    INDEX idx_to_from (to_user_id, from_user_id),
    INDEX idx_created_at (created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='消息表';

-- 好友关系表
CREATE TABLE IF NOT EXISTS friends (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    user_id BIGINT UNSIGNED NOT NULL COMMENT '用户ID',
    friend_id BIGINT UNSIGNED NOT NULL COMMENT '好友ID',
    remark VARCHAR(64) NOT NULL DEFAULT '' COMMENT '备注名',
    status TINYINT NOT NULL DEFAULT 1 COMMENT '状态: 0=待确认, 1=已添加, 2=已删除',
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY uk_user_friend (user_id, friend_id),
    INDEX idx_user_id (user_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='好友关系表';

-- 群组表
CREATE TABLE IF NOT EXISTS groups (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    group_name VARCHAR(64) NOT NULL COMMENT '群名称',
    owner_id BIGINT UNSIGNED NOT NULL COMMENT '群主ID',
    description VARCHAR(256) NOT NULL DEFAULT '' COMMENT '群描述',
    created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_owner_id (owner_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='群组表';

-- 群成员表
CREATE TABLE IF NOT EXISTS group_members (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    group_id BIGINT UNSIGNED NOT NULL COMMENT '群ID',
    user_id BIGINT UNSIGNED NOT NULL COMMENT '用户ID',
    role TINYINT NOT NULL DEFAULT 0 COMMENT '角色: 0=成员, 1=管理员, 2=群主',
    joined_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY uk_group_user (group_id, user_id),
    INDEX idx_group_id (group_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='群成员表';

-- 插入测试数据
INSERT INTO users (username, password, salt, nickname) VALUES
('test1', 'e10adc3949ba59abbe56e057f20f883e', 'testsalt', '测试用户1'),
('test2', 'e10adc3949ba59abbe56e057f20f883e', 'testsalt', '测试用户2')
ON DUPLICATE KEY UPDATE username=username;

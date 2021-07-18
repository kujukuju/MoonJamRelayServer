server {
    listen 8008 http2 ssl;
    server_name *.moonlightprincess.io;

    ssl_certificate /etc/letsencrypt/live/relay.veraegames.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/relay.veraegames.com/privkey.pem;
    # ssl_dhparam /etc/letsencrypt/live/relay.veraegames.com/dh.pem;
    ssl_protocols TLSv1 TLSv1.1 TLSv1.2;
    ssl_ciphers HIGH:!aNULL:!MD5;
    ssl_prefer_server_ciphers on;

    # WebSocket support
    # proxy_http_version 1.1;
    # proxy_set_header Upgrade $http_upgrade;
    # proxy_set_header Connection "upgrade";

    location / {
        # prevents 502 bad gateway error
        proxy_buffers 8 32k;
        proxy_buffer_size 64k;

        proxy_pass http://localhost:58008;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;

        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";

        proxy_read_timeout 600s;
    }
}

server {
    listen 8007 http2 ssl;
    server_name *.moonlightprincess.io;

    ssl_certificate /etc/letsencrypt/live/relay.veraegames.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/relay.veraegames.com/privkey.pem;
    # ssl_dhparam /etc/letsencrypt/live/relay.veraegames.com/dh.pem;
    ssl_protocols TLSv1 TLSv1.1 TLSv1.2;
    ssl_ciphers HIGH:!aNULL:!MD5;
    ssl_prefer_server_ciphers on;

    # WebSocket support
    # proxy_http_version 1.1;
    # proxy_set_header Upgrade $http_upgrade;
    # proxy_set_header Connection "upgrade";

    location / {
        # prevents 502 bad gateway error
        proxy_buffers 8 32k;
        proxy_buffer_size 64k;

        proxy_pass http://localhost:58007;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;

        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";

        proxy_read_timeout 600s;
    }
}

server {
    listen 8006 http2 ssl;
    server_name *.moonlightprincess.io;

    ssl_certificate /etc/letsencrypt/live/relay.veraegames.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/relay.veraegames.com/privkey.pem;
    # ssl_dhparam /etc/letsencrypt/live/relay.veraegames.com/dh.pem;
    ssl_protocols TLSv1 TLSv1.1 TLSv1.2;
    ssl_ciphers HIGH:!aNULL:!MD5;
    ssl_prefer_server_ciphers on;

    # WebSocket support
    # proxy_http_version 1.1;
    # proxy_set_header Upgrade $http_upgrade;
    # proxy_set_header Connection "upgrade";

    location / {
        # prevents 502 bad gateway error
        proxy_buffers 8 32k;
        proxy_buffer_size 64k;

        proxy_pass http://localhost:58006;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;

        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";

        proxy_read_timeout 600s;
    }
}

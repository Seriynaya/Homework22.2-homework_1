from django.contrib.auth.views import LoginView, LogoutView
from django.urls import path
from users.apps import UsersConfig
from users.views import UsersCreateView, email_verification

app_name = UsersConfig.name


urlpatterns = [
    path('login/', LoginView.as_view(template_name='login.html'), name='login'),
    path('logout/', LogoutView.as_view(), name='logout'),
    path('register/', UsersCreateView.as_view(), name='register'),
    path('email-confirm/<int:token>/', email_verification, name='email_confirm'),
]
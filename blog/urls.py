from django.urls import path
from blog.apps import BlogConfig
from blog.views import PublicationsListView, PublicationsDetailView, PublicationsUpdateView

app_name = BlogConfig.name

urlpatterns = [
    path('', PublicationsListView.as_view(), name='publications_list'),
    path('publications_detail/<int:pk>', PublicationsDetailView.as_view(), name='publications_detail'),
    path('publications_list/<int:pk>/update', PublicationsUpdateView.as_view(), name='publications_update'),
]
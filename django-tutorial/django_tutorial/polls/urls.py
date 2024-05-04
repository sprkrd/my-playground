from django.urls import path


from . import views


urlpatterns = [
    # e.g.: /polls/
    path("", views.index, name="index"),
    # e.g.: /polls/5/
    path("<int:question_id>/", views.detail, name="detail"),
    # e.g.: /polls/5/results/
    path("<int:question_id>/results/", views.results, name="results"),
    # e.g.: /polls/5/votes/
    path("<int:question_id>/vote/", views.vote, name="vote"),

]
